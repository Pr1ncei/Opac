#include "menu.h"
#include "../utils/Logger.h"
#include "../utils/EventBus.h"
#include <iostream>
#include <limits>

// ============================================================================
//  mainMenu
// ============================================================================
void Menu::mainMenu()
{
    EventBus& bus = EventBus::getInstance();

    bus.subscribe("LOGIN_SUCCESS", [](const Event& e) {
        (void)e;
        });
    bus.subscribe("BOOK_BORROWED", [](const Event& e) {
        Logger::getInstance().info("[EVENT] BOOK_BORROWED — " + e.payload);
        });
    bus.subscribe("BOOK_RETURNED", [](const Event& e) {
        Logger::getInstance().info("[EVENT] BOOK_RETURNED — " + e.payload);
        });
    bus.subscribe("BOOK_ADDED", [](const Event& e) {
        Logger::getInstance().info("[EVENT] BOOK_ADDED   — " + e.payload);
        });
    bus.subscribe("BOOK_DELETED", [](const Event& e) {
        Logger::getInstance().info("[EVENT] BOOK_DELETED — " + e.payload);
        });

    int choice;
    do
    {
        std::cout << "\n=================================\n";
        std::cout << "        OPAC LIBRARY SYSTEM\n";
        std::cout << "=================================\n";
        std::cout << "[1] Login\n";
        std::cout << "[2] Register\n";
        std::cout << "[3] Exit\n";
        std::cout << "---------------------------------\n";

        choice = readInt("Enter choice: ");

        switch (choice)
        {
        case 1: loginMenu();    break;
        case 2: registerMenu(); break;
        case 3: std::cout << "Goodbye!\n"; break;
        default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 3);
}

// ============================================================================
//  loginMenu
// ============================================================================
void Menu::loginMenu()
{
    std::string username, password;

    std::cout << "\n========== LOGIN ==========\n";
    std::cout << "Username (0 cancel): ";
    std::cin >> username;
    if (username == "0") return;

    std::cout << "Password (0 cancel): ";
    std::cin >> password;
    if (password == "0") return;

    AuthService auth;
    Account account = auth.login(username, password);

    if (!account.isEmpty())
    {
        std::cout << "\nWelcome, " << account.getUsername() << "!\n";

        if (account.isAdmin())
            adminMenu(account.getUsername());
        else
            userMenu(account.getUsername());
    }
    else
    {
        std::cout << "Invalid username or password.\n";
    }
}

// ============================================================================
//  registerMenu
// ============================================================================
void Menu::registerMenu()
{
    std::string username, password, confirm;
    AuthService auth;

    std::cout << "\n========== REGISTER ==========\n";

    // Username loop.
    while (true)
    {
        std::cout << "Username (0 cancel): ";
        std::cin >> username;
        if (username == "0") return;

        if (username.length() < 3)
        {
            std::cout << "Username must be at least 3 characters.\n";
            continue;
        }
        if (auth.userExists(username))
        {
            std::cout << "Username already taken.\n";
            continue;
        }
        break;
    }

    // Password loop.
    while (true)
    {
        std::cout << "Password (0 cancel): ";
        std::cin >> password;
        if (password == "0") return;

        if (password.length() < 4)
        {
            std::cout << "Password must be at least 4 characters.\n";
            continue;
        }

        std::cout << "Confirm Password: ";
        std::cin >> confirm;

        if (password != confirm)
        {
            std::cout << "Passwords do not match.\n";
            continue;
        }
        break;
    }

    if (auth.registerUser(username, password, "user"))
        std::cout << "Account created! You may now log in.\n";
    else
        std::cout << "Registration failed. Please try again.\n";
}

// ============================================================================
//  adminMenu
// ============================================================================
void Menu::adminMenu(const std::string& username)
{
    BookManager   bookMgr;
    BorrowManager borrowMgr;

    int choice;
    do
    {
        std::cout << "\n=================================\n";
        std::cout << "     ADMIN MENU (" << username << ")\n";
        std::cout << "=================================\n";
        std::cout << "[1]  Add Book\n";
        std::cout << "[2]  Edit Book\n";
        std::cout << "[3]  Delete Book\n";
        std::cout << "[4]  View All Books\n";
        std::cout << "[5]  Search Book\n";
        std::cout << "[6]  View All Borrowed Books\n";
        std::cout << "[7]  Logout\n";
        std::cout << "---------------------------------\n";

        choice = readInt("Enter choice: ");

        switch (choice)
        {
        case 1: bookMgr.addBook();                break;
        case 2: bookMgr.editBook();               break;
        case 3: bookMgr.deleteBook();             break;
        case 4: bookMgr.viewBooks("admin");       break;
        case 5: bookMgr.searchBook();             break;
        case 6: borrowMgr.viewBorrowedBooks(""); break;  // "" = admin view
        case 7: std::cout << "Logged out.\n";    break;
        default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 7);
}

// ============================================================================
//  userMenu
// ============================================================================
void Menu::userMenu(const std::string& username)
{
    BookManager   bookMgr;
    BorrowManager borrowMgr;

    int choice;
    do
    {
        std::cout << "\n=================================\n";
        std::cout << "     USER MENU (" << username << ")\n";
        std::cout << "=================================\n";
        std::cout << "[1] View Books\n";
        std::cout << "[2] Search Book\n";
        std::cout << "[3] Borrow Book\n";
        std::cout << "[4] Return Book\n";
        std::cout << "[5] My Borrowed Books\n";
        std::cout << "[6] Logout\n";
        std::cout << "---------------------------------\n";

        choice = readInt("Enter choice: ");

        switch (choice)
        {
        case 1: bookMgr.viewBooks("user");             break;
        case 2: bookMgr.searchBook();                  break;
        case 3: borrowMgr.borrowBook(username);        break;
        case 4: borrowMgr.returnBook(username);        break;
        case 5: borrowMgr.viewBorrowedBooks(username); break;
        case 6: std::cout << "Logged out.\n";          break;
        default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 6);
}

// ============================================================================
//  Input helpers
// ============================================================================

// Safely reads an integer, loops on invalid input.
int Menu::readInt(const std::string& prompt)
{
    int val;
    while (true)
    {
        std::cout << prompt;
        std::cin >> val;
        if (!std::cin.fail()) break;
        clearInputError();
        std::cout << "Please enter a number.\n";
    }
    return val;
}

void Menu::clearInputError()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}