#pragma once
#include <string>
#include <vector>
#include "../../db/Database.h"
#include "../../models/Book.h"

namespace sql {
    class ResultSet;
}

class BookManager
{
public:
    void addBook();
    void editBook();
    void deleteBook();
    void viewBooks(const std::string& role);   // role="admin"|"user"
    void searchBook();

    // Returns a Book object for bookId. Throws if not found.
    Book getBookById(int bookId);

    // Decrements available_copies.  Returns false if none left.
    bool decrementAvailableCopies(int bookId);

    // Increments available_copies.  Returns false on overflow.
    bool incrementAvailableCopies(int bookId);

    // Print a formatted single-book detail block.
    void showBookDetails(int bookId);

private:
    Database db_;

    void printTableHeader(const std::string& role) const;
    void printBookRow(sql::ResultSet* res, const std::string& role) const;
};