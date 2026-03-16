#include "Database.h"
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

using namespace std;

Database::Database() {

    try {

        driver = sql::mysql::get_mysql_driver_instance();

        con = driver->connect(
            "tcp://127.0.0.1:3306",
            "root",
            ""
        );

        con->setSchema("opac_db");

    }
    catch (sql::SQLException& e) {

        cout << "Database connection failed: " << e.what() << endl;
        con = nullptr;
    }
}

sql::Connection* Database::getConnection() {
    return con;
}