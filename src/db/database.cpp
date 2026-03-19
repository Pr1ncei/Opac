#include "Database.h"
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

using namespace std;

sql::Connection* Database::getConnection() {
    if (!con) {
        throw std::runtime_error("[DATABASE.H] No database connection available");
    }
    return con;
}

Database::~Database() {
    if (con)
    {
        delete con; 
        con = nullptr; 
    }
}