#pragma once
#include <string>
#include <ctime>

class BorrowRecord
{
public:
    BorrowRecord() = default;

    BorrowRecord(int borrowId,
                 int bookId,
                 const std::string& bookTitle,
                 const std::string& username,
                 const std::string& borrowDate,
                 const std::string& dueDate,
                 const std::string& returnDate = "");

    // Getters
    int                getBorrowId()   const { return borrowId_; }
    int                getBookId()     const { return bookId_; }
    const std::string& getBookTitle()  const { return bookTitle_; }
    const std::string& getUsername()   const { return username_; }
    const std::string& getBorrowDate() const { return borrowDate_; }
    const std::string& getDueDate()    const { return dueDate_; }
    const std::string& getReturnDate() const { return returnDate_; }

    bool isReturned() const { return !returnDate_.empty(); }

    // Fee calculation
    // Overdue fee = PHP 20 per day beyond due date.
    static constexpr double FEE_PER_DAY = 20.0;

    int    calcOverdueDays(const std::string& actualReturnDate) const;
    double calcFee(const std::string& actualReturnDate) const;

private:
    int         borrowId_   = 0;
    int         bookId_     = 0;
    std::string bookTitle_;
    std::string username_;
    std::string borrowDate_;
    std::string dueDate_;
    std::string returnDate_;

    static std::tm parseDate(const std::string& dateStr);
};
