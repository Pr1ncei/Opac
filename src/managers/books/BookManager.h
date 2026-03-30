#pragma once
#include <string>
#include <vector>
#include "../../db/Database.h"
#include "../../models/Book.h"

namespace sql {
    class ResultSet;
}

// ─── GUI data structures ──────────────────────────────────────────────────────

struct BookRow
{
    int         bookId;
    std::string title;
    std::string author;
    std::string dewey;
    int         totalCopies;
    int         availableCopies;
    bool        isAvailable() const { return availableCopies > 0; }
};

struct BookStats
{
    int totalBooks;       // distinct titles
    int totalCopies;      // SUM(total_copies)
    int availableCopies;  // SUM(available_copies)
    int borrowedCopies;   // totalCopies - availableCopies
};

// ─────────────────────────────────────────────────────────────────────────────

class BookManager
{
public:
    // ── Existing CLI methods (unchanged) ─────────────────────────────────────
    void addBook();
    void editBook();
    void deleteBook();
    void viewBooks(const std::string& role);   // role="admin"|"user"
    void searchBook();

    //// Returns a Book object for bookId. Throws if not found.
    Book getBookById(int bookId);

    //// Decrements available_copies.  Returns false if none left.
    bool decrementAvailableCopies(int bookId);

    //// Increments available_copies.  Returns false on overflow.
    bool incrementAvailableCopies(int bookId);

    // Print a formatted single-book detail block.
    void showBookDetails(int bookId);

    // ── GUI methods (no std::cin / std::cout) ─────────────────────────────────

    // Returns every book ordered by title.
    std::vector<BookRow> getAllBooks();

    // Returns books matching keyword across title / author / dewey / id.
    // Passing an empty string is equivalent to getAllBooks().
    std::vector<BookRow> searchBooksGui(const std::string& keyword);

    // Returns aggregate stats for the admin dashboard header row.
    BookStats getStats();

    // Inserts a new book. Returns true on success.
    bool addBookGui(const std::string& title,
        const std::string& author,
        const std::string& dewey,
        int copies);

    // Updates a book by ID. Pass an empty string to keep a text field unchanged;
    // pass totalCopies < 1 to keep the existing copy count.
    // Returns true on success, false if ID not found or DB error.
    bool editBookGui(int bookId,
        const std::string& title,
        const std::string& author,
        const std::string& dewey,
        int totalCopies);

    // Deletes a book by ID. Returns true on success.
    bool deleteBook(int bookId);

private:
    Database db_;
    void printTableHeader(const std::string& role) const;
    void printBookRow(sql::ResultSet* res, const std::string& role) const;
};