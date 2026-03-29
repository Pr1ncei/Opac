#include "FeeCalculator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <regex>

// parseDateToTime
std::time_t FeeCalculator::parseDateToTime(const std::string& dateStr)
{
    std::tm t = {};
    std::istringstream ss(dateStr);

    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail())
    {
        ss.clear();
        ss.str(dateStr);
        ss >> std::get_time(&t, "%Y-%m-%d");
        if (ss.fail())
            throw std::invalid_argument(
                "[FeeCalculator] Cannot parse date: " + dateStr);
    }

    t.tm_hour = 0; t.tm_min = 0; t.tm_sec = 0;
    t.tm_isdst = -1;
    return std::mktime(&t);
}

// overdueDays
int FeeCalculator::overdueDays(const std::string& dueDate,
    const std::string& actualReturnDate)
{
    try
    {
        std::time_t tDue = parseDateToTime(dueDate);
        std::time_t tRet = parseDateToTime(actualReturnDate);

        if (tDue == (std::time_t)-1 || tRet == (std::time_t)-1) return 0;

        double diffSec = std::difftime(tRet, tDue);
        int    diffDays = static_cast<int>(diffSec / 86400.0);

        return (diffDays > 0) ? diffDays : 0;
    }
    catch (...) { return 0; }
}

// overdueFee
double FeeCalculator::overdueFee(int days)
{
    return (days > 0) ? days * FEE_PER_DAY : 0.0;
}

// totalFee
double FeeCalculator::totalFee(const std::string& dueDate,
                               const std::string& actualReturnDate)
{
    return overdueFee(overdueDays(dueDate, actualReturnDate));
}

// promptReturnDate
std::string FeeCalculator::promptReturnDate(const std::string& dueDate)
{
    std::cout << "\n  Borrow period : " << BORROW_PERIOD_DAYS << " days\n";
    std::cout << "  Due date      : " << dueDate << "\n";
    std::cout << "  Overdue fee   : ₱" << FEE_PER_DAY << " per day\n";

    std::string dateStr;
    std::regex  datePattern(R"(\d{4}-\d{2}-\d{2})");

    while (true)
    {
        std::cout << "\n  Enter intended return date (YYYY-MM-DD): ";
        std::cin >> dateStr;

        if (!std::regex_match(dateStr, datePattern))
        {
            std::cout << "  Invalid format. Use YYYY-MM-DD.\n";
            continue;
        }

        // Preview fee
        int    days = overdueDays(dueDate, dateStr);
        double fee  = overdueFee(days);

        std::cout << "\n";
        if (days > 0)
        {
            std::cout << "  Your intended return is " << days
                      << " day(s) OVERDUE.\n";
            std::cout << "  Estimated fee: ₱" << std::fixed
                      << std::setprecision(2) << fee << "\n";
        }
        else
        {
            std::cout << "  Return is within the allowed period. No fee.\n";
        }

        char confirm;
        std::cout << "  Confirm this return date? (y/n): ";
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') break;
    }

    return dateStr;
}

// printFeeReceipt
void FeeCalculator::printFeeReceipt(const std::string& bookTitle,
                                    const std::string& dueDate,
                                    const std::string& returnDate,
                                    int    days,
                                    double fee)
{
    std::cout << "\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "            RETURN RECEIPT\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "  Book      : " << bookTitle   << "\n";
    std::cout << "  Due Date  : " << dueDate     << "\n";
    std::cout << "  Returned  : " << returnDate  << "\n";
    std::cout << std::string(50, '-') << "\n";

    if (days > 0)
    {
        std::cout << "  Overdue   : " << days << " day(s)\n";
        std::cout << "  Fee Rate  : PHP" << FeeCalculator::FEE_PER_DAY << "/day\n";
        std::cout << "  TOTAL FEE : PHP" << std::fixed
                  << std::setprecision(2) << fee << "\n";
    }
    else
    {
        std::cout << "  Status    : Returned on time — No fee!\n";
    }

    std::cout << std::string(50, '=') << "\n";
}
