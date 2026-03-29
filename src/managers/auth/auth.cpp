#include "auth.h"
#include "../../utils/Logger.h"
#include "../../utils/EventBus.h"
#include <iostream>
#include <cppconn/prepared_statement.h>

// login
Account AuthService::login(const std::string& username,
                           const std::string& password)
{
    try
    {
        sql::Connection* con = db_.getConnection();

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT role FROM users WHERE username=? AND password=?"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            std::string role = res->getString("role");
            Logger::getInstance().info("LOGIN OK - user: " + username +
                                       "  role: " + role);

            EventBus::getInstance().publish("LOGIN_SUCCESS",
                                            "username=" + username +
                                            ";role=" + role);
            return Account(username, password, role);
        }

        Logger::getInstance().warning("LOGIN FAIL - user: " + username);
        EventBus::getInstance().publish("LOGIN_FAILED", "username=" + username);
        return Account();
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(std::string("[AuthService::login] ") +
                                    e.what());
        return Account();
    }
}

// registerUser
bool AuthService::registerUser(const std::string& username,
                                const std::string& password,
                                const std::string& role)
{
    try
    {
        if (userExists(username))
        {
            Logger::getInstance().warning(
                "REGISTER FAIL - username taken: " + username);
            return false;
        }

        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO users (username, password, role) VALUES (?,?,?)"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->setString(3, role);
        pstmt->execute();

        Logger::getInstance().info("REGISTER OK - user: " + username +
                                   "  role: " + role);
        EventBus::getInstance().publish("USER_REGISTERED",
                                        "username=" + username);
        return true;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[AuthService::registerUser] ") + e.what());
        return false;
    }
}

// userExists
bool AuthService::userExists(const std::string& username)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT 1 FROM users WHERE username=?"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        return res->next();
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[AuthService::userExists] ") + e.what());
        return false;
    }
}
