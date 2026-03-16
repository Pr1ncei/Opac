#ifndef DATABASE_H
#define DATABASE_H

#include <mysql_driver.h>
#include <mysql_connection.h>

class Database {

private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

public:
    Database();
    sql::Connection* getConnection();
};

#endif