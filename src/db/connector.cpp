#include "db/connector.h"

#include <fmt/format.h>

namespace parallel_db::db
{
std::unique_ptr< pqxx::connection > Connector::getConnection( const Settings& settings )
{
    return std::make_unique< pqxx::connection >( fmt::format( "postgresql://{}:{}@{}:{}/{}",
                                                              settings.getDbUserName(),
                                                              settings.getDbUserPassword(),
                                                              settings.getDbIp(),
                                                              settings.getDbPort(),
                                                              settings.getDbName() ) );
}
} // namespace parallel_db::db
