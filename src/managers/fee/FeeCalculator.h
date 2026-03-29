#pragma once
#include <string>
#include <ctime>
#include <string>

class FeeCalculator
{
public:
    // Borrowing period: 1 week (7 days)
    static constexpr int    BORROW_PERIOD_DAYS = 7;
    static constexpr double FEE_PER_DAY        = 20.0;

    // Core calculations

    static int    overdueDays(const std::string& dueDate,
                              const std::string& actualReturnDate);

    // Total overdue fee for a number of overdue days.
    static double overdueFee(int overdueDays);

    static double totalFee(const std::string& dueDate,
                           const std::string& actualReturnDate);

    // Ask user for intended return date, display fee preview.
    static std::string promptReturnDate(const std::string& dueDate);

    // Display a formatted fee receipt.
    static void printFeeReceipt(const std::string& bookTitle,
                                const std::string& dueDate,
                                const std::string& returnDate,
                                int    overdueDays,
                                double fee);

private:
    static std::time_t parseDateToTime(const std::string& dateStr);
};
