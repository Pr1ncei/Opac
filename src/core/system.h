#ifndef SYSTEM_H
#define SYSTEM_H

#include "../db/Database.h"
#include "../models/Book.h"
#include "../models/Account.h"
#include <string>

using namespace std;

class System {

private:
    Database db;

public:

    void run();

    void login();
    void registerUser();

    void adminMenu(string username);
    void userMenu(string username);

    void showBookDetails(int id);

    void addBook();
    void editBook();
    void deleteBook();
    void viewBooks(string role);
    void searchBook();

    void borrowBook(string username);
    void returnBook(string username);
    void viewBorrowedBooks(string username);
};

#endif