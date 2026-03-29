#pragma once
#include <string>

class Book
{
public:
    Book();

    Book(int id,
        const std::string& title,
        const std::string& author,
        const std::string& dewey,
        int totalCopies,
        int availableCopies);

    // Getters
    int                getBookId()         const { return bookId_; }
    const std::string& getTitle()          const { return title_; }
    const std::string& getAuthor()         const { return author_; }
    const std::string& getDewey()          const { return dewey_; }
    int                getTotalCopies()    const { return totalCopies_; }
    int                getAvailableCopies()const { return availableCopies_; }

    bool isAvailable() const { return availableCopies_ > 0; }

    // Mutators
    void setTitle(const std::string& t) { title_ = t; }
    void setAuthor(const std::string& a) { author_ = a; }
    void setDewey(const std::string& d) { dewey_ = d; }
    void setTotalCopies(int n) { totalCopies_ = n; }
    void setAvailableCopies(int n) { availableCopies_ = n; }

    // Decrease / increase available copies — returns false if impossible.
    bool checkOut();
    bool checkIn();

    // Display
    void printSummary() const;

private:
    int         bookId_ = 0;
    std::string title_;
    std::string author_;
    std::string dewey_;
    int         totalCopies_ = 1;
    int         availableCopies_ = 1;
};