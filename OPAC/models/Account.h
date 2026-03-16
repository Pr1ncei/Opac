#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

using namespace std;

class Account {

public:
    string username;
    string password;
    string role;

    Account() {}

    Account(string u, string p, string r) {
        username = u;
        password = p;
        role = r;
    }
};

#endif