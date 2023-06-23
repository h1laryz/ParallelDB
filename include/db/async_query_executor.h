#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <pqxx/pqxx>

namespace parallel_db::db
{
class AsyncQueryExecutor
{
public:
    explicit AsyncQueryExecutor( const std::uint32_t& threads_amount );
    [[nodiscard]] std::vector< pqxx::result > query( std::string query );

private:
    [[nodiscard]] std::string getTableName( const std::string& query ) const;
    [[nodiscard]] std::uint64_t getAmountOfRows( const std::string& table_name ) const;

private:
    std::uint32_t threads_amount_{};
    boost::asio::thread_pool thread_pool_;
};
} // namespace parallel_db::db