#include <csignal>
#include <iostream>
#include <memory>
#include <string>

#include <boost/program_options.hpp>
#include <fmtlog/fmtlog.h>

#include "db/async_query_executor.h"
#include "db/query_executor.h"
#include "utils/timer.h"

namespace
{
constexpr auto kThreads{ "threads" };

std::function< void( int ) > shutdown_handler;
void signalHandler( int signal )
{
    shutdown_handler( signal );
}

std::shared_ptr< boost::program_options::options_description >
program_options( const int argc, const char**& argv, std::uint32_t& threads_amount, bool& is_help )
{
    // clang-format off
    auto desc { std::make_shared<boost::program_options::options_description>( "Parallel db settings") };
    desc->add_options()
      ( kThreads,
        boost::program_options::value< std::uint32_t >(&threads_amount),
        "(required) thread amount" )
      ( "help, h",
        boost::program_options::bool_switch( &is_help ),
        "display a help dialog" );
    // clang-format on
    boost::program_options::variables_map map_of_args;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, *desc ),
                                   map_of_args );
    boost::program_options::notify( map_of_args );

    return desc;
}
} // namespace

int main( int argc, char** argv )
{
    std::uint32_t threads_amount{};
    bool is_help{};

    const auto desc{
        program_options( argc, const_cast< const char**& >( argv ), threads_amount, is_help )
    };

    if ( is_help || threads_amount == 0 )
    {
        std::cout << *desc << std::endl;
        return EXIT_SUCCESS;
    }

    shutdown_handler = [ & ]( int signal [[maybe_unused]] )
    {
        logi( "You have stopped program by signal" );
        return EXIT_SUCCESS;
    };
    std::signal( SIGINT | SIGTERM, signalHandler );

    fmtlog::startPollingThread();

    parallel_db::utils::Timer timer;
    std::string query;
    std::cout << "to quit: \\q" << std::endl;

    while ( true )
    {
        std::cout << "query: ";
        getline( std::cin, query );
        if ( query == "\\q" )
        {
            break;
        }

        if ( !query.empty() )
        {
            try
            {
                timer.reset();
                if ( threads_amount > 1 )
                {
                    static parallel_db::db::AsyncQueryExecutor query_executor{ threads_amount };

                    const auto result{ query_executor.query( query ) };
                    for ( auto item_result{ result.cbegin() }; item_result != result.cend();
                          ++item_result )
                    {
                        for ( auto row{ item_result->cbegin() }; row != item_result->cend(); ++row )
                        {
                            for ( auto field{ row->cbegin() }; field != row->cend(); ++field )
                            {
                                std::cout << field->c_str() << '\t';
                            }
                            std::cout << '\n';
                        }
                    }
                }
                else
                {
                    static parallel_db::db::QueryExecutor parallel_query_executor;
                    const auto result{ parallel_query_executor.query( query ) };
                    for ( auto row{ result.cbegin() }; row != result.cend(); ++row )
                    {
                        for ( auto field{ row->cbegin() }; field != row->cend(); ++field )
                        {
                            std::cout << field->c_str() << '\t';
                        }
                        std::cout << '\n';
                    }
                }
                logi( "Execution time: {} ms", timer.getTime() );
            }
            catch ( const std::exception& ex )
            {
                loge( "query execution error: {}", ex.what() );
            }
            catch ( ... )
            {
                loge( "query execution error: unknown error" );
            }
        }
    }

    return EXIT_SUCCESS;
}
