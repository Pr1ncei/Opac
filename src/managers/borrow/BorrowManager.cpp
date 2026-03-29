#include "BorrowManager.h"
#include "../../utils/Logger.h"
#include "../../utils/EventBus.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

// ============================================================================
//  borrowBook
// ============================================================================
void BorrowManager::borrowBook(const std::string& username)
{
    std::string input;
    std::cout << "\n========== BORROW BOOK ==========\n";
    std::cout << "Enter Book ID (0 cancel): ";
    std::cin >> input;
    if (input == "0") { std::cout << "Cancelled.\n"; return; }

    int bookId;
    try { bookId = std::stoi(input); }
    catch (...) { std::cout << "Invalid ID.\n"; return; }

    bookMgr_.showBookDetails(bookId);

    Book book;
    try { book = bookMgr_.getBookById(bookId); }
    catch (std::exception& e)
    {
        std::cout << e.what() << "\n";
        return;
    }

    if (!book.isAvailable())
    {
        std::cout << "Sorry, all copies of this book are currently borrowed.\n";
        return;
    }

    std::cout << "\n--- Borrow Terms ---\n";
    std::cout << "Borrow period : " << FeeCalculator::BORROW_PERIOD_DAYS
        << " days from today\n";
    std::cout << "Overdue fee   : PHP" << FeeCalculator::FEE_PER_DAY
        << " per day\n";

    char confirm;
    std::cout << "Proceed with borrowing? (y/n): ";
    std::cin >> confirm;
    if (confirm != 'y' && confirm != 'Y')
    {
        std::cout << "Borrow cancelled.\n";
        return;
    }

    try
    {
        sql::Connection* con = db_.getConnection();

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO borrow_records "
                "(book_id, username, borrow_date, due_date) "
                "VALUES (?, ?, NOW(), DATE_ADD(NOW(), INTERVAL ? DAY))"));
        pstmt->setInt(1, bookId);
        pstmt->setString(2, username);
        pstmt->setInt(3, FeeCalculator::BORROW_PERIOD_DAYS);
        pstmt->execute();

        if (!bookMgr_.decrementAvailableCopies(bookId))
        {
            std::cout << "Could not update inventory. Please try again.\n";
            return;
        }

        Logger::getInstance().info(
            "BORROW: user=" + username +
            " book_id=" + std::to_string(bookId));

        EventBus::getInstance().publish("BOOK_BORROWED",
            "username=" + username +
            ";book_id=" + std::to_string(bookId));

        std::cout << "\nBook borrowed successfully!\n";
        std::cout << "Return within " << FeeCalculator::BORROW_PERIOD_DAYS
            << " days to avoid fees.\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::borrowBook] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

// ============================================================================
//  returnBook
// ============================================================================
void BorrowManager::returnBook(const std::string& username)
{
    std::string input;
    std::cout << "\n========== RETURN BOOK ==========\n";
    std::cout << "Enter Book ID to return (0 cancel): ";
    std::cin >> input;
    if (input == "0") { std::cout << "Cancelled.\n"; return; }

    int bookId;
    try { bookId = std::stoi(input); }
    catch (...) { std::cout << "Invalid ID.\n"; return; }

    bookMgr_.showBookDetails(bookId);

    try
    {
        sql::Connection* con = db_.getConnection();

        // Fetch the active borrow record for this user + book.
        std::unique_ptr<sql::PreparedStatement> sel(
            con->prepareStatement(
                "SELECT borrow_id, due_date "
                "FROM borrow_records "
                "WHERE book_id=? AND username=? AND return_date IS NULL "
                "ORDER BY borrow_date DESC LIMIT 1"));
        sel->setInt(1, bookId);
        sel->setString(2, username);
        std::unique_ptr<sql::ResultSet> res(sel->executeQuery());

        if (!res->next())
        {
            std::cout << "No active borrow record found for this book.\n";
            return;
        }

        int         borrowId = res->getInt("borrow_id");
        std::string dueDate = res->getString("due_date");

        std::string returnDate = FeeCalculator::promptReturnDate(dueDate);

        int    days = FeeCalculator::overdueDays(dueDate, returnDate);
        double fee = FeeCalculator::overdueFee(days);

        std::string bookTitle = "Unknown";
        try { bookTitle = bookMgr_.getBookById(bookId).getTitle(); }
        catch (...) {}

        FeeCalculator::printFeeReceipt(bookTitle, dueDate, returnDate, days, fee);

        if (fee > 0.0)
        {
            char pay;
            std::cout << "\nPlease pay PHP" << std::fixed
                << std::setprecision(2) << fee
                << " to the librarian.\n";
            std::cout << "Confirm payment received? (y/n): ";
            std::cin >> pay;
            if (pay != 'y' && pay != 'Y')
            {
                std::cout << "Return on hold until payment is confirmed.\n";
                return;
            }
        }

        std::unique_ptr<sql::PreparedStatement> upd(
            con->prepareStatement(
                "UPDATE borrow_records "
                "SET return_date=?, overdue_days=?, fee=? "
                "WHERE borrow_id=?"));
        upd->setString(1, returnDate);
        upd->setInt(2, days);
        upd->setDouble(3, fee);
        upd->setInt(4, borrowId);
        upd->executeUpdate();

        bookMgr_.incrementAvailableCopies(bookId);

        Logger::getInstance().info(
            "RETURN: user=" + username +
            " book_id=" + std::to_string(bookId) +
            " overdue_days=" + std::to_string(days) +
            " fee=" + std::to_string(fee));

        EventBus::getInstance().publish("BOOK_RETURNED",
            "username=" + username +
            ";book_id=" + std::to_string(bookId) +
            ";fee=" + std::to_string(fee));

        std::cout << "Book returned successfully. Thank you!\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::returnBook] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

// ============================================================================
//  viewBorrowedBooks
// ============================================================================
void BorrowManager::viewBorrowedBooks(const std::string& username)
{
    bool isAdmin = username.empty();

    try
    {
        sql::Connection* con = db_.getConnection();

        std::unique_ptr<sql::PreparedStatement> pstmt;
        if (isAdmin)
        {
            pstmt.reset(con->prepareStatement(
                "SELECT br.borrow_id, br.book_id, b.title, br.username, "
                "       br.borrow_date, br.due_date, br.return_date, "
                "       br.overdue_days, br.fee "
                "FROM borrow_records br "
                "JOIN books b ON br.book_id = b.book_id "
                "WHERE br.return_date IS NULL "
                "ORDER BY br.due_date ASC"));
        }
        else
        {
            pstmt.reset(con->prepareStatement(
                "SELECT br.borrow_id, br.book_id, b.title, br.username, "
                "       br.borrow_date, br.due_date, br.return_date, "
                "       br.overdue_days, br.fee "
                "FROM borrow_records br "
                "JOIN books b ON br.book_id = b.book_id "
                "WHERE br.username=? AND br.return_date IS NULL "
                "ORDER BY br.due_date ASC"));
            pstmt->setString(1, username);
        }

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        printBorrowHeader(isAdmin);
        bool found = false;

        while (res->next())
        {
            found = true;
            std::cout << std::left
                << std::setw(8) << res->getInt("borrow_id")
                << std::setw(35) << res->getString("title")
                << std::setw(20) << res->getString("due_date");

            if (isAdmin)
                std::cout << std::setw(15) << res->getString("username");

            std::string due = res->getString("due_date");
            std::time_t now = std::time(nullptr);
            std::tm* tmNow = std::localtime(&now);
            char buf[20];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", tmNow);
            std::string today(buf);

            std::cout << (due < today ? " OVERDUE" : " On time") << "\n";
        }

        if (!found)
            std::cout << "  No active borrow records found.\n";

        std::cout << std::string(80, '=') << "\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::viewBorrowedBooks] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

// Private helper
void BorrowManager::printBorrowHeader(bool isAdmin) const
{
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << std::left
        << std::setw(8) << "ID"
        << std::setw(35) << "Title"
        << std::setw(20) << "Due Date";
    if (isAdmin)
        std::cout << std::setw(15) << "Borrowed By";
    std::cout << "Status\n";
    std::cout << std::string(80, '=') << "\n";
}