#pragma once
#include <string>
#include "../../db/Database.h"
#include "../books/BookManager.h"
#include "../fee/FeeCalculator.h"

class BorrowManager
{
public:
    // Interactive console menus
    void borrowBook(const std::string& username);
    void returnBook(const std::string& username);
    void viewBorrowedBooks(const std::string& username); // "" = all (admin)

private:
    Database     db_;
    BookManager  bookMgr_;

    // Shared helper: prints borrow record table header.
    void printBorrowHeader(bool isAdmin) const;
};