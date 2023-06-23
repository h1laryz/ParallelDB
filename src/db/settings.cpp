#include "db/settings.h"

#include <boost/foreach.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace parallel_db::db
{
Settings::Settings()
{
    readSettingsFile();
}

void Settings::readSettingsFile()
{
    boost::property_tree::ptree json;
    boost::property_tree::read_json( "./settings.json", json );

    db_ip_            = json.get_child( "psql.ip" ).get_value< std::string >();
    db_port_          = json.get_child( "psql.port" ).get_value< std::uint16_t >();
    db_name_          = json.get_child( "psql.name" ).get_value< std::string >();
    db_user_name_     = json.get_child( "psql.user_name" ).get_value< std::string >();
    db_user_password_ = json.get_child( "psql.user_password" ).get_value< std::string >();
}

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}

std::string Settings::getDbIp() const
{
    return db_ip_;
}

std::string Settings::getDbName() const
{
    return db_name_;
}

std::uint16_t Settings::getDbPort() const
{
    return db_port_;
}

std::string Settings::getDbUserName() const
{
    return db_user_name_;
}

std::string Settings::getDbUserPassword() const
{
    return db_user_password_;
}
} // namespace parallel_db::db