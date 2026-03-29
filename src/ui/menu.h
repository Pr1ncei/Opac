#pragma once
#include <string>
#include <iostream>
#include "../managers/auth/auth.h"
#include "../managers/books/BookManager.h"
#include "../managers/borrow/BorrowManager.h"

class Menu
{
public:
    void mainMenu();

private:
    // Sub-menus
    void loginMenu();
    void registerMenu();
    void adminMenu(const std::string& username);
    void userMenu(const std::string& username);

    // Input helpers
    static int  readInt(const std::string& prompt);
    static void clearInputError();
};