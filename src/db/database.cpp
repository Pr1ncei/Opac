#include "Database.h"
#include "../utils/Logger.h"
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

Database::Database()
{
    connect();
}

Database::~Database()
{
    if (con_)
    {
        delete con_;
        con_ = nullptr;
    }
}

sql::Connection* Database::getConnection()
{
    if (!con_)
        throw std::runtime_error("[Database] No active database connection.");
    return con_;
}

void Database::connect()
{
    auto env = loadEnv(".env");

    const std::string server = env.count("DB_SERVER") ? env["DB_SERVER"] : "tcp://127.0.0.1:3306";
    const std::string username = env.count("DB_USERNAME") ? env["DB_USERNAME"] : "";
    const std::string password = env.count("DB_PASSWORD") ? env["DB_PASSWORD"] : "";

    try
    {
        driver_ = sql::mysql::get_mysql_driver_instance();
        con_ = driver_->connect(server, username, password);
        con_->setSchema("opac_db");

        Logger::getInstance().info("Connected to MySQL - schema: opac_db");
    }
    catch (sql::SQLException& e)
    {
        Logger::getInstance().error(
            std::string("[Database] Connection failed: ") + e.what() +
            " (code=" + std::to_string(e.getErrorCode()) + ")");
        con_ = nullptr;
        throw std::runtime_error(
            std::string("[Database] Connection failed: ") + e.what());
    }
}