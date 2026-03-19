#ifndef DATABASE_H
#define DATABASE_H

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <stdexcept>
#include <iostream>
#include "../utils/env.hpp"    

class Database 
{

private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

   

public:
    Database()
    {
        auto env = loadEnv(".env");
        // please create a .env file for this 
        const std::string server = env["DB_SERVER"];
        const std::string username = env["DB_USERNAME"];
        const std::string password = env["DB_PASSWORD"];

        try 
        {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(server, username, password);
            std::cout << "Connected to MySQL server successfully!" << std::endl;

            con->setSchema("opac_db");
            std::cout << "Database 'opac_db' selected successfully!" << std::endl;
        }
        catch (sql::SQLException& e)
        {
            std::cerr << "SQL Exception: " << e.what()
                << " (MySQL error code: " << e.getErrorCode()
                << ", SQLState: " << e.getSQLState() << ")" << std::endl;
            con = nullptr; 
            throw std::runtime_error(std::string("[DATABASE.H] Database connection failed: ") + e.what());
        }
    }
    ~Database();
    sql::Connection* getConnection();
};

#endif