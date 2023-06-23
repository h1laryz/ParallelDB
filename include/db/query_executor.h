#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <pqxx/pqxx>

namespace parallel_db::db
{
class QueryExecutor
{
public:
    QueryExecutor() = default;
    [[nodiscard]] pqxx::result query( const std::string& query );
};
} // namespace parallel_db::db