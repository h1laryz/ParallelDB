#pragma once

#include <pqxx/pqxx>

#include "db/settings.h"

namespace parallel_db::db
{
class Connector
{
public:
    static std::unique_ptr< pqxx::connection > getConnection( const Settings& settings );
};
} // namespace parallel_db::db