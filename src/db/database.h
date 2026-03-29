#pragma once
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <stdexcept>
#include <iostream>
#include "../utils/env.hpp"

class Database
{
public:
    Database();

    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    sql::Connection* getConnection();

private:
    sql::mysql::MySQL_Driver* driver_ = nullptr;
    sql::Connection* con_ = nullptr;

    void connect();
};