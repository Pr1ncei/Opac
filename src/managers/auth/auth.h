#pragma once
#include <string>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "../../db/Database.h"
#include "../../models/Account.h"

class AuthService
{
public:
    // Returns an Account with the user's role, or an empty Account on failure.
    Account login(const std::string& username, const std::string& password);

    // Registers a new user. Returns false if username already exists.
    bool registerUser(const std::string& username,
                      const std::string& password,
                      const std::string& role = "user");

    // Returns true if the username exists in the database.
    bool userExists(const std::string& username);

private:
    Database db_;
};
