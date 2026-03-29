#include "Book.h"
#include <iostream>
#include <iomanip>

Book::Book()
    : bookId_(0), title_(""), author_(""), dewey_(""),
    totalCopies_(1), availableCopies_(1)
{
}

Book::Book(int id,
    const std::string& title,
    const std::string& author,
    const std::string& dewey,
    int totalCopies,
    int availableCopies)
    : bookId_(id), title_(title), author_(author), dewey_(dewey),
    totalCopies_(totalCopies), availableCopies_(availableCopies)
{
}

// Returns false if no copies are available to check out.
bool Book::checkOut()
{
    if (availableCopies_ <= 0) return false;
    --availableCopies_;
    return true;
}

// Returns false if all copies are already in
bool Book::checkIn()
{
    if (availableCopies_ >= totalCopies_) return false;
    ++availableCopies_;
    return true;
}

void Book::printSummary() const
{
    std::cout << std::left
        << std::setw(6) << bookId_
        << std::setw(35) << title_
        << std::setw(25) << author_
        << std::setw(12) << dewey_
        << availableCopies_ << "/" << totalCopies_
        << (isAvailable() ? "  [Available]" : "  [Fully Borrowed]")
        << "\n";
}