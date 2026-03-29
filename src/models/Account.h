#pragma once
#include <string>

class Account
{
public:
    Account() = default;

    Account(const std::string& username,
        const std::string& password,
        const std::string& role)
        : username_(username), password_(password), role_(role) {
    }

    const std::string& getUsername() const { return username_; }
    const std::string& getPassword() const { return password_; }
    const std::string& getRole()     const { return role_; }

    bool isAdmin() const { return role_ == "admin"; }
    bool isEmpty() const { return username_.empty(); }

private:
    std::string username_;
    std::string password_;
    std::string role_;
};