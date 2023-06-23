#include "db/async_query_executor.h"

#define OFFSET_LIMIT

#include <algorithm>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <fmtlog/fmtlog.h>

#include "db/connector.h"
#include "db/settings.h"
#include "utils/string.h"

namespace parallel_db::db
{
AsyncQueryExecutor::AsyncQueryExecutor( const std::uint32_t& threads_amount )
    : threads_amount_{ threads_amount }
    , thread_pool_{ threads_amount }
{
}

std::vector< pqxx::result > AsyncQueryExecutor::query( std::string query )
{
    std::vector< pqxx::result > result;
    result.reserve( threads_amount_ );

    query = utils::copyRemoveSemicolon( query );
    const auto table_name{ getTableName( query ) };
    const auto amount_of_rows{ getAmountOfRows( table_name ) };
    const auto amount_for_task{ amount_of_rows / threads_amount_ };
    const auto extra{ amount_of_rows % amount_for_task };

    for ( std::uint32_t i{ 0 }; i < threads_amount_; ++i )
    {
        boost::asio::post(
            thread_pool_,
            [ query, i, extra, amount_for_task, &result ]()
            {
                const auto amount_for_this_task{ i > extra ? amount_for_task
                                                           : amount_for_task + 1 };
                const auto offset{ i * amount_for_task };
                const auto connection{ Connector::getConnection( Settings::getInstance() ) };
                pqxx::transaction tx{ *connection };

#ifdef OFFSET_LIMIT
                result.emplace_back( tx.exec( fmt::format( "{} OFFSET {} LIMIT {};",
                                                           query,
                                                           offset,
                                                           amount_for_this_task ) ) );
#else
                if ( boost::ifind_first( query, " WHERE" ) )
                {
                    result.emplace_back( tx.exec( fmt::format( "{} AND (id > {} AND id <= {});",
                                                               query,
                                                               offset,
                                                               amount_for_this_task + offset ) ) );
                }
                else
                {
                    result.emplace_back( tx.exec( fmt::format( "{} WHERE id > {} AND id <= {};",
                                                               query,
                                                               offset,
                                                               amount_for_this_task + offset ) ) );
                }
#endif
                tx.commit();
            } );
    }

    thread_pool_.join();
    return result;
}

std::string AsyncQueryExecutor::getTableName( const std::string& query ) const
{
    std::vector< std::string > query_words;
    boost::split( query_words, utils::copyRemoveWhitespaces( query ), boost::is_any_of( " " ) );

    std::string table_name;
    const auto from_it{ std::find_if( query_words.cbegin(),
                                      query_words.cend(),
                                      []( const std::string& str )
                                      {
                                          return boost::iequals( str, "from" );
                                      } ) };

    if ( from_it == query_words.cend() || from_it + 1 == query_words.cend() )
    {
        throw std::runtime_error( fmt::format( "Can't find table in query: {}", query ) );
    }

    return *( from_it + 1 );
}

std::uint64_t AsyncQueryExecutor::getAmountOfRows( const std::string& table_name ) const
{
    const auto connection{ Connector::getConnection( Settings::getInstance() ) };
    pqxx::transaction tx{ *connection };
    const auto [ result ]{ tx.query1< std::uint64_t >(
        fmt::format( "SELECT COUNT(*) FROM {}", table_name ) ) };
    return result;
}

} // namespace parallel_db::db