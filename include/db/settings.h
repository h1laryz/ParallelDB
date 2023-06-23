#pragma once

#include <memory>

#include <boost/json.hpp>

namespace parallel_db::db
{
class Settings
{
public:
    static Settings& getInstance();

    [[nodiscard]] std::string getDbName() const;
    [[nodiscard]] std::string getDbIp() const;
    [[nodiscard]] std::uint16_t getDbPort() const;
    [[nodiscard]] std::string getDbUserName() const;
    [[nodiscard]] std::string getDbUserPassword() const;

protected:
    explicit Settings();

private:
    void readSettingsFile();

    std::string db_name_;
    std::string db_ip_;
    std::uint16_t db_port_{};
    std::string db_user_name_;
    std::string db_user_password_;
};
} // namespace parallel_db::db