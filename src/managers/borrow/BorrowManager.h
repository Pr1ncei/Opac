#pragma once
#include <string>
#include <vector>
#include "../../db/Database.h"
#include "../books/BookManager.h"
#include "../fee/FeeCalculator.h"

// ─── GUI data structure ───────────────────────────────────────────────────────

struct BorrowRow
{
    int         borrowId;
    int         bookId;
    std::string bookTitle;
    std::string username;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;   // empty = not yet returned
    int         overdueDays;
    double      fee;

    // True if the item is currently overdue (compares dueDate against today
    // for unreturned books, or checks overdueDays > 0 for returned ones).
    bool isOverdue() const;
};

// ─────────────────────────────────────────────────────────────────────────────

class BorrowManager
{
public:
    // ── Existing CLI methods (unchanged) ─────────────────────────────────────
    void borrowBook(const std::string& username);
    void returnBook(const std::string& username);
    void viewBorrowedBooks(const std::string& username);   // "" = all (admin)

    // ── GUI methods (no std::cin / std::cout) ─────────────────────────────────

    // Inserts a borrow record and decrements available_copies.
    // Returns false if no copies are available or on DB error.
    bool borrowBookGui(int bookId, const std::string& username);

    // Records a return.
    //   returnDate     : "YYYY-MM-DD"
    //   confirmPayment : caller must pass true after the user acknowledges any fee;
    //                    if fee > 0 and confirmPayment is false the call is a no-op.
    // Returns true on success.
    bool returnBookGui(int bookId,
        const std::string& username,
        const std::string& returnDate,
        bool confirmPayment);

    // Returns the single active (unreturned) borrow record for one user + book.
    // borrowId == -1 signals "not found".
    BorrowRow getActiveBorrow(int bookId, const std::string& username);

    // All unreturned records for one user — drives the user dashboard table.
    std::vector<BorrowRow> getActiveBorrowsForUser(const std::string& username);

    // All unreturned records across every user — drives the admin dashboard tab.
    std::vector<BorrowRow> getAllActiveBorrows();

private:
    Database     db_;
    BookManager  bookMgr_;

    void printBorrowHeader(bool isAdmin) const;
};