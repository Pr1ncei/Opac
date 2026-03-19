#include "System.h"
#include <iostream>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

void System::run() {

    int choice;

    do {

        cout << "\n=================================\n";
        cout << "        LIBRARY SYSTEM\n";
        cout << "=================================\n";
        cout << "[1] Login\n";
        cout << "[2] Register\n";
        cout << "[3] Exit\n";
        cout << "---------------------------------\n";
        cout << "Enter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input!\n";
            continue;
        }

        switch (choice) {

        case 1: login(); break;
        case 2: registerUser(); break;
        case 3: cout << "Exiting system...\n"; break;
        default: cout << "Invalid choice.\n";
        }

    } while (choice != 3);
}

void System::login()
{
    try
    {
        string username, password;

        cout << "\n========== LOGIN ==========\n";
        cout << "Username (0 to cancel): ";
        cin >> username;

        if (username == "0") return;

        cout << "Password (0 to cancel): ";
        cin >> password;

        if (password == "0") return;

        sql::Connection* con = db.getConnection();
        sql::Statement* stmt = con->createStatement();

        sql::ResultSet* res = stmt->executeQuery(
            "SELECT * FROM users WHERE username='" + username +
            "' AND password='" + password + "'"
        );

        if (res->next())
        {
            string role = res->getString("role");

            cout << "\nWelcome " << username << endl;

            if (role == "admin")
                adminMenu(username);
            else
                userMenu(username);
        }
        else
        {
            cout << "Invalid login.\n";
        }

        delete res;
        delete stmt;
    }
    catch (exception& e)
    {
        cout << "Login error: " << e.what() << endl;
    }
}

void System::registerUser()
{
    try
    {
        string username, password, confirm;

        cout << "\n========== REGISTER ==========\n";
        cout << "Enter Username (0 to cancel): ";
        cin >> username;

        if (username == "0") return;

        if (username.empty())
        {
            cout << "Username cannot be empty.\n";
            return;
        }

        sql::Connection* con = db.getConnection();
        if (!con) {
            cout << "Database connection failed.\n";
            return;
        }
        sql::Statement* stmt = con->createStatement();

        sql::ResultSet* res = stmt->executeQuery(
            "SELECT * FROM users WHERE username='" + username + "'"
        );

        if (res->next())
        {
            cout << "Username already exists.\n";
            delete res;
            delete stmt;
            return;
        }

        delete res;

        cout << "Enter Password (0 to cancel): ";
        cin >> password;

        if (password == "0") return;

        cout << "Confirm Password: ";
        cin >> confirm;

        if (password != confirm)
        {
            cout << "Passwords do not match.\n";
            delete stmt;
            return;
        }

        stmt->execute(
            "INSERT INTO users VALUES('" + username + "','" + password + "','user')"
        );

        cout << "Account created successfully.\n";

        delete stmt;
    }
    catch (exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
}

void System::adminMenu(string username) {

    int choice;

    do {

        cout << "\n=================================\n";
        cout << "            ADMIN MENU\n";
        cout << "=================================\n";
        cout << "[1] Add Book\n";
        cout << "[2] Edit Book\n";
        cout << "[3] Delete Book\n";
        cout << "[4] View Books\n";
        cout << "[5] Search Book\n";
        cout << "[6] View Borrowed Books\n";
        cout << "[7] Logout\n";
        cout << "---------------------------------\n";
        cout << "Enter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input!\n";
            continue;
        }

        switch (choice) {

        case 1: addBook(); break;
        case 2: editBook(); break;
        case 3: deleteBook(); break;
        case 4: viewBooks("admin"); break;
        case 5: searchBook(); break;
        case 6: viewBorrowedBooks(""); break;
        case 7: cout << "Logging out...\n"; break;
        default: cout << "Invalid choice.\n";
        }

    } while (choice != 7);
}

void System::userMenu(string username) {

    int choice;

    do {

        cout << "\n=================================\n";
        cout << "             USER MENU\n";
        cout << "=================================\n";
        cout << "[1] View Books\n";
        cout << "[2] Search Book\n";
        cout << "[3] Borrow Book\n";
        cout << "[4] Return Book\n";
        cout << "[5] View My Borrowed Books\n";
        cout << "[6] Logout\n";
        cout << "---------------------------------\n";
        cout << "Enter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input!\n";
            continue;
        }

        switch (choice) {

        case 1: viewBooks("user"); break;
        case 2: searchBook(); break;
        case 3: borrowBook(username); break;
        case 4: returnBook(username); break;
        case 5: viewBorrowedBooks(username); break;
        case 6: cout << "Logging out...\n"; break;
        default: cout << "Invalid choice.\n";
        }

    } while (choice != 6);
}

void System::showBookDetails(int id)
{
    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    sql::ResultSet* res = stmt->executeQuery(
        "SELECT * FROM books WHERE book_id=" + to_string(id)
    );

    if (!res->next())
    {
        cout << "Book not found.\n";
        delete res;
        delete stmt;
        return;
    }

    cout << "\n===============================================================\n";
    cout << "ID | Title | Author | Dewey\n";
    cout << "===============================================================\n";

    cout << res->getInt("book_id") << " | "
        << res->getString("title") << " | "
        << res->getString("author") << " | "
        << res->getString("dewey") << endl;

    cout << "===============================================================\n";

    delete res;
    delete stmt;
}

void System::addBook() {

    string title, author, dewey;

    cin.ignore();

    cout << "\n========== ADD BOOK ==========\n";
    cout << "Enter Title (0 to cancel): ";
    getline(cin, title);

    if (title == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    cout << "Enter Author (0 to cancel): ";
    getline(cin, author);

    if (author == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    cout << "Enter Dewey Decimal (0 to cancel): ";
    getline(cin, dewey);

    if (dewey == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    stmt->execute(
        "INSERT INTO books(title,author,dewey,available) VALUES('" +
        title + "','" + author + "','" + dewey + "',1)"
    );

    cout << "Book added successfully.\n";

    delete stmt;
}

void System::editBook() {

    string input;
    int id;

    cout << "\n========== EDIT BOOK ==========\n";
    cout << "Enter Book ID to edit (0 to cancel): ";
    cin >> input;

    if (input == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    id = stoi(input);

    showBookDetails(id);

    cout << "\n";

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    sql::ResultSet* res = stmt->executeQuery(
        "SELECT * FROM books WHERE book_id=" + to_string(id)
    );

    if (!res->next()) {
        cout << "Book ID not found.\n";
        delete res;
        delete stmt;
        return;
    }

    string oldTitle = res->getString("title");
    string oldAuthor = res->getString("author");
    string oldDewey = res->getString("dewey");

    cin.ignore();

    string title, author, dewey;

    cout << "Current Title: " << oldTitle << endl;
    cout << "New Title (Enter to keep / 0 to cancel): ";
    getline(cin, title);

    if (title == "0") {
        cout << "Operation cancelled.\n";
        delete res;
        delete stmt;
        return;
    }

    if (title.empty())
        title = oldTitle;

    cout << "Current Author: " << oldAuthor << endl;
    cout << "New Author (Enter to keep / 0 to cancel): ";
    getline(cin, author);

    if (author == "0") {
        cout << "Operation cancelled.\n";
        delete res;
        delete stmt;
        return;
    }

    if (author.empty())
        author = oldAuthor;

    cout << "Current Dewey Decimal: " << oldDewey << endl;
    cout << "New Dewey Decimal (Enter to keep / 0 to cancel): ";
    getline(cin, dewey);

    if (dewey == "0") {
        cout << "Operation cancelled.\n";
        delete res;
        delete stmt;
        return;
    }

    if (dewey.empty())
        dewey = oldDewey;

    int rows = stmt->executeUpdate(
        "UPDATE books SET title='" + title +
        "', author='" + author +
        "', dewey='" + dewey +
        "' WHERE book_id=" + to_string(id)
    );

    if (rows > 0)
        cout << "Book updated successfully.\n";
    else
        cout << "Update failed.\n";

    delete res;
    delete stmt;
}

void System::deleteBook() {

    string input;

    cout << "\n========== DELETE BOOK ==========\n";
    cout << "Enter Book ID to delete (0 to cancel): ";
    cin >> input;

    if (input == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    int id = stoi(input);

    showBookDetails(id);

    cout << "\n";

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    cout << "Are you sure you want to delete this book? (y/n): ";

    char confirm;
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        cout << "Delete cancelled.\n";
        delete stmt;
        return;
    }

    int rows = stmt->executeUpdate(
        "DELETE FROM books WHERE book_id=" + to_string(id)
    );

    if (rows > 0)
        cout << "Book deleted successfully.\n";
    else
        cout << "Book ID not found.\n";

    delete stmt;
}

void System::viewBooks(string role) {

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM books");

    cout << "\n===============================================================\n";

    if (role == "admin")
        cout << "ID | Title | Author | Dewey | Status | Borrowed By\n";
    else
        cout << "ID | Title | Author | Dewey | Status\n";

    cout << "===============================================================\n";

    while (res->next()) {

        cout << res->getInt("book_id") << " | "
            << res->getString("title") << " | "
            << res->getString("author") << " | "
            << res->getString("dewey") << " | ";

        if (res->getBoolean("available"))
            cout << "Available";
        else
            cout << "Borrowed";

        if (role == "admin") {

            string borrower = res->getString("borrowed_by");

            if (borrower == "")
                borrower = "-";

            cout << " | " << borrower;
        }

        cout << endl;
    }

    cout << "===============================================================\n";

    delete res;
    delete stmt;
}

void System::searchBook() {

    string keyword;

    cin.ignore();

    cout << "\n========== SEARCH BOOK ==========\n";
    cout << "Enter Title / Author / Dewey / Book ID (0 to cancel): ";
    getline(cin, keyword);

    if (keyword == "0") {
        cout << "Operation cancelled.\n";
        return;
    }

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    sql::ResultSet* res = stmt->executeQuery(
        "SELECT * FROM books WHERE "
        "title LIKE '%" + keyword + "%' OR "
        "author LIKE '%" + keyword + "%' OR "
        "dewey LIKE '%" + keyword + "%' OR "
        "book_id LIKE '%" + keyword + "%'"
    );

    cout << "\n===============================================================\n";
    cout << "ID | Title | Author | Dewey | Status\n";
    cout << "===============================================================\n";

    bool found = false;

    while (res->next()) {

        found = true;

        cout << res->getInt("book_id") << " | "
            << res->getString("title") << " | "
            << res->getString("author") << " | "
            << res->getString("dewey") << " | ";

        if (res->getBoolean("available"))
            cout << "Available\n";
        else
            cout << "Borrowed\n";
    }

    if (!found)
        cout << "No matching books found.\n";

    cout << "===============================================================\n";

    delete res;
    delete stmt;
}

void System::borrowBook(string username)
{
    try
    {
        string input;
        cout << "\n========== BORROW BOOK ==========\n";
        cout << "Enter Book ID to borrow (0 cancel): ";
        cin >> input;

        if (input == "0") {
            cout << "Operation cancelled.\n";
            return;
        }

        int id = stoi(input);

        showBookDetails(id);

        cout << "\n";

        sql::Connection* con = db.getConnection();
        sql::Statement* stmt = con->createStatement();

        stmt->execute(
            "UPDATE books SET "
            "available=0,"
            "borrowed_by='" + username + "',"
            "borrow_date=NOW(),"
            "due_date=DATE_ADD(NOW(), INTERVAL 14 DAY)"
            " WHERE book_id=" + to_string(id) + " AND available=1"
        );

        cout << "Book borrowed successfully.\n";
        cout << "Return within 14 days.\n";

        delete stmt;
    }
    catch (exception& e)
    {
        cout << "Borrow error: " << e.what() << endl;
    }
}

void System::returnBook(string username)
{
    try
    {
        string input;
        cout << "\n========== RETURN BOOK ==========\n";
        cout << "Enter Book ID to return (0 cancel): ";
        cin >> input;

        if (input == "0") {
            cout << "Operation cancelled.\n";
            return;
        }

        int id = stoi(input);

        showBookDetails(id);

        cout << "\n";

        sql::Connection* con = db.getConnection();
        sql::Statement* stmt = con->createStatement();

        sql::ResultSet* res = stmt->executeQuery(
            "SELECT due_date FROM books WHERE book_id=" + to_string(id)
        );

        if (res->next())
        {
            stmt->execute(
                "UPDATE books SET "
                "available=1,"
                "borrowed_by=NULL,"
                "return_date=NOW() "
                "WHERE book_id=" + to_string(id)
            );

            cout << "Book returned successfully.\n";
            cout << "Late fees may apply if overdue.\n";
        }

        delete res;
        delete stmt;
    }
    catch (exception& e)
    {
        cout << "Return error: " << e.what() << endl;
    }
}

void System::viewBorrowedBooks(string username) {

    sql::Connection* con = db.getConnection();
    sql::Statement* stmt = con->createStatement();

    string query;

    if (username == "")
        query = "SELECT * FROM books WHERE available=0";
    else
        query = "SELECT * FROM books WHERE borrowed_by='" + username + "'";

    sql::ResultSet* res = stmt->executeQuery(query);

    cout << "\n===============================================================\n";
    cout << "ID | Title | Author | Dewey\n";
    cout << "===============================================================\n";

    bool found = false;

    while (res->next()) {

        found = true;

        cout << res->getInt("book_id") << " | "
            << res->getString("title") << " | "
            << res->getString("author") << " | "
            << res->getString("dewey");

        if (username == "")
            cout << " | Borrowed by: " << res->getString("borrowed_by");

        cout << endl;
    }

    if (!found)
        cout << "No borrowed books found.\n";

    cout << "===============================================================\n";

    delete res;
}