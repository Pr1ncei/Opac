#include "BorrowRecord.h"
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <iomanip>

BorrowRecord::BorrowRecord(int borrowId,
                           int bookId,
                           const std::string& bookTitle,
                           const std::string& username,
                           const std::string& borrowDate,
                           const std::string& dueDate,
                           const std::string& returnDate)
    : borrowId_(borrowId), bookId_(bookId), bookTitle_(bookTitle),
      username_(username), borrowDate_(borrowDate),
      dueDate_(dueDate), returnDate_(returnDate)
{}

// Fee calculation
std::tm BorrowRecord::parseDate(const std::string& dateStr)
{
    std::tm t = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail())
    {
        ss.clear();
        ss.str(dateStr);
        ss >> std::get_time(&t, "%Y-%m-%d");
    }
    t.tm_hour = 0; t.tm_min = 0; t.tm_sec = 0;
    t.tm_isdst = -1;
    return t;
}

int BorrowRecord::calcOverdueDays(const std::string& actualReturnDate) const
{
    if (dueDate_.empty() || actualReturnDate.empty()) return 0;

    try
    {
        std::tm due    = parseDate(dueDate_);
        std::tm ret    = parseDate(actualReturnDate);
        std::time_t tDue = std::mktime(&due);
        std::time_t tRet = std::mktime(&ret);

        if (tDue == (std::time_t)-1 || tRet == (std::time_t)-1) return 0;

        double diffSec  = std::difftime(tRet, tDue);
        int    diffDays = static_cast<int>(diffSec / 86400.0);
        return (diffDays > 0) ? diffDays : 0;
    }
    catch (...) { return 0; }
}

// Overdue fee = PHP 20 × overdue days
double BorrowRecord::calcFee(const std::string& actualReturnDate) const
{
    return calcOverdueDays(actualReturnDate) * FEE_PER_DAY;
}
