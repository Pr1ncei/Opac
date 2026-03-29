#include "BookManager.h"
#include "../../utils/Logger.h"
#include "../../utils/EventBus.h"
#include <iostream>
#include <iomanip>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <stdexcept>

// ============================================================================
//  INTERACTIVE MENUS
// ============================================================================

void BookManager::addBook()
{
    std::string title, author, dewey, copiesStr;
    int copies = 1;

    std::cin.ignore();

    std::cout << "\n========== ADD BOOK ==========\n";
    std::cout << "Title   (0 cancel): "; std::getline(std::cin, title);
    if (title == "0") { std::cout << "Cancelled.\n"; return; }

    std::cout << "Author  (0 cancel): "; std::getline(std::cin, author);
    if (author == "0") { std::cout << "Cancelled.\n"; return; }

    std::cout << "Dewey   (0 cancel): "; std::getline(std::cin, dewey);
    if (dewey == "0") { std::cout << "Cancelled.\n"; return; }

    std::cout << "Number of copies [default 1]: ";
    std::getline(std::cin, copiesStr);
    if (!copiesStr.empty() && copiesStr != "0")
    {
        try { copies = std::stoi(copiesStr); }
        catch (...) { copies = 1; }
    }
    if (copies < 1) copies = 1;

    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO books (title, author, dewey, total_copies, available_copies)"
                " VALUES (?, ?, ?, ?, ?)"));
        pstmt->setString(1, title);
        pstmt->setString(2, author);
        pstmt->setString(3, dewey);
        pstmt->setInt(4, copies);
        pstmt->setInt(5, copies);
        pstmt->execute();

        Logger::getInstance().info("BOOK ADDED: " + title);
        EventBus::getInstance().publish("BOOK_ADDED", "title=" + title);
        std::cout << "Book added successfully (" << copies << " cop"
            << (copies == 1 ? "y" : "ies") << ").\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::addBook] ") + e.what());
        std::cout << "Error adding book: " << e.what() << "\n";
    }
}

void BookManager::editBook()
{
    std::string input;
    std::cout << "\n========== EDIT BOOK ==========\n";
    std::cout << "Book ID (0 cancel): ";
    std::cin >> input;
    if (input == "0") { std::cout << "Cancelled.\n"; return; }

    int id;
    try { id = std::stoi(input); }
    catch (...) { std::cout << "Invalid ID.\n"; return; }

    showBookDetails(id);

    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT title, author, dewey, total_copies"
                " FROM books WHERE book_id=?"));
        pstmt->setInt(1, id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) { std::cout << "Book not found.\n"; return; }

        std::string oldTitle = res->getString("title");
        std::string oldAuthor = res->getString("author");
        std::string oldDewey = res->getString("dewey");
        int         oldCopies = res->getInt("total_copies");

        std::cin.ignore();
        std::string title, author, dewey, copiesStr;
        int copies = oldCopies;

        std::cout << "Current Title  : " << oldTitle << "\nNew (Enter=keep, 0=cancel): ";
        std::getline(std::cin, title);
        if (title == "0") { std::cout << "Cancelled.\n"; return; }
        if (title.empty())  title = oldTitle;

        std::cout << "Current Author : " << oldAuthor << "\nNew (Enter=keep, 0=cancel): ";
        std::getline(std::cin, author);
        if (author == "0") { std::cout << "Cancelled.\n"; return; }
        if (author.empty()) author = oldAuthor;

        std::cout << "Current Dewey  : " << oldDewey << "\nNew (Enter=keep, 0=cancel): ";
        std::getline(std::cin, dewey);
        if (dewey == "0") { std::cout << "Cancelled.\n"; return; }
        if (dewey.empty())  dewey = oldDewey;

        std::cout << "Current Copies : " << oldCopies << "\nNew (Enter=keep): ";
        std::getline(std::cin, copiesStr);
        if (!copiesStr.empty())
        {
            try { copies = std::stoi(copiesStr); if (copies < 1) copies = oldCopies; }
            catch (...) { copies = oldCopies; }
        }

        std::unique_ptr<sql::PreparedStatement> upd(
            con->prepareStatement(
                "UPDATE books SET title=?, author=?, dewey=?, total_copies=?"
                " WHERE book_id=?"));
        upd->setString(1, title);
        upd->setString(2, author);
        upd->setString(3, dewey);
        upd->setInt(4, copies);
        upd->setInt(5, id);
        int rows = upd->executeUpdate();

        if (rows > 0)
        {
            Logger::getInstance().info(
                "BOOK EDITED: id=" + std::to_string(id));
            EventBus::getInstance().publish("BOOK_EDITED",
                "book_id=" + std::to_string(id));
            std::cout << "Book updated successfully.\n";
        }
        else std::cout << "Update failed.\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::editBook] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

void BookManager::deleteBook()
{
    std::string input;
    std::cout << "\n========== DELETE BOOK ==========\n";
    std::cout << "Book ID (0 cancel): ";
    std::cin >> input;
    if (input == "0") { std::cout << "Cancelled.\n"; return; }

    int id;
    try { id = std::stoi(input); }
    catch (...) { std::cout << "Invalid ID.\n"; return; }

    showBookDetails(id);

    char confirm;
    std::cout << "Are you sure? (y/n): ";
    std::cin >> confirm;
    if (confirm != 'y' && confirm != 'Y')
    {
        std::cout << "Delete cancelled.\n";
        return;
    }

    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "DELETE FROM books WHERE book_id=?"));
        pstmt->setInt(1, id);
        int rows = pstmt->executeUpdate();

        if (rows > 0)
        {
            Logger::getInstance().info(
                "BOOK DELETED: id=" + std::to_string(id));
            EventBus::getInstance().publish("BOOK_DELETED",
                "book_id=" + std::to_string(id));
            std::cout << "Book deleted.\n";
        }
        else std::cout << "Book ID not found.\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::deleteBook] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

void BookManager::viewBooks(const std::string& role)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT * FROM books ORDER BY title"));

        printTableHeader(role);
        while (res->next())
            printBookRow(res.get(), role);

        std::cout << std::string(80, '=') << "\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::viewBooks] ") + e.what());
        std::cout << "Error loading books: " << e.what() << "\n";
    }
}

void BookManager::searchBook()
{
    std::string keyword;
    std::cin.ignore();
    std::cout << "\n========== SEARCH BOOK ==========\n";
    std::cout << "Title / Author / Dewey / ID (0 cancel): ";
    std::getline(std::cin, keyword);
    if (keyword == "0") { std::cout << "Cancelled.\n"; return; }

    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT * FROM books WHERE "
                "title LIKE ? OR author LIKE ? OR dewey LIKE ? OR "
                "CAST(book_id AS CHAR) LIKE ?"));
        std::string kw = "%" + keyword + "%";
        pstmt->setString(1, kw);
        pstmt->setString(2, kw);
        pstmt->setString(3, kw);
        pstmt->setString(4, kw);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        printTableHeader("user");
        bool found = false;
        while (res->next()) { found = true; printBookRow(res.get(), "user"); }
        if (!found) std::cout << "  No matching books found.\n";
        std::cout << std::string(80, '=') << "\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::searchBook] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

// ============================================================================
//  NON-INTERACTIVE HELPERS
// ============================================================================

Book BookManager::getBookById(int bookId)
{
    sql::Connection* con = db_.getConnection();
    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->prepareStatement(
            "SELECT * FROM books WHERE book_id=?"));
    pstmt->setInt(1, bookId);
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    if (!res->next())
        throw std::runtime_error("Book ID " + std::to_string(bookId) + " not found.");

    return Book(
        res->getInt("book_id"),
        res->getString("title"),
        res->getString("author"),
        res->getString("dewey"),
        res->getInt("total_copies"),
        res->getInt("available_copies")
    );
}

bool BookManager::decrementAvailableCopies(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE books SET available_copies = available_copies - 1"
                " WHERE book_id=? AND available_copies > 0"));
        pstmt->setInt(1, bookId);
        return pstmt->executeUpdate() > 0;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::decrementAvailableCopies] ") + e.what());
        return false;
    }
}

bool BookManager::incrementAvailableCopies(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE books SET available_copies = available_copies + 1"
                " WHERE book_id=? AND available_copies < total_copies"));
        pstmt->setInt(1, bookId);
        return pstmt->executeUpdate() > 0;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::incrementAvailableCopies] ") + e.what());
        return false;
    }
}

void BookManager::showBookDetails(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT * FROM books WHERE book_id=?"));
        pstmt->setInt(1, bookId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) { std::cout << "Book not found.\n"; return; }

        std::cout << "\n" << std::string(60, '-') << "\n";
        std::cout << "  Book ID : " << res->getInt("book_id") << "\n";
        std::cout << "  Title   : " << res->getString("title") << "\n";
        std::cout << "  Author  : " << res->getString("author") << "\n";
        std::cout << "  Dewey   : " << res->getString("dewey") << "\n";
        std::cout << "  Copies  : " << res->getInt("available_copies")
            << " available / " << res->getInt("total_copies")
            << " total\n";
        std::cout << std::string(60, '-') << "\n";
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::showBookDetails] ") + e.what());
        std::cout << "Error: " << e.what() << "\n";
    }
}

// ============================================================================
//  PRIVATE HELPERS
// ============================================================================

void BookManager::printTableHeader(const std::string& role) const 
{
    std::cout << "\n" << std::string(90, '=') << "\n";
    std::cout << std::left
        << std::setw(6) << "ID"
        << std::setw(30) << "Title"
        << std::setw(20) << "Author"
        << std::setw(10) << "Dewey"
        << "Status (Avail/Total)\n";
    std::cout << std::string(90, '=') << "\n";
}

void BookManager::printBookRow(sql::ResultSet* res, const std::string& role) const {
    int avail = res->getInt("available_copies");
    int total = res->getInt("total_copies");

    std::cout << std::left
        << std::setw(6) << res->getInt("book_id")
        << std::setw(30) << res->getString("title").substr(0, 28) // Truncate long titles
        << std::setw(20) << res->getString("author").substr(0, 18)
        << std::setw(10) << res->getString("dewey")
        << avail << "/" << total
        << (avail > 0 ? "  [Available]" : "  [OUT]")
        << "\n";
}