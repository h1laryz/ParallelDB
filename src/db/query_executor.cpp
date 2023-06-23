#include "db/query_executor.h"

#include <string>

#include <boost/algorithm/string.hpp>

#include "db/connector.h"
#include "db/settings.h"

namespace parallel_db::db
{
pqxx::result QueryExecutor::query( const std::string& query )
{
    const auto connection{ Connector::getConnection( Settings::getInstance() ) };
    pqxx::transaction tx{ *connection };

    const auto result{ tx.exec( query ) };

    tx.commit();

    return result;
}
} // namespace parallel_db::db