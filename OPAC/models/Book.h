#ifndef BOOK_H
#define BOOK_H

#include <string>

using namespace std;

class Book {

public:

    int id;
    string title;
    string author;
    string dewey;
    bool available;
    string borrowedBy;

    Book();
    Book(int i, string t, string a, string d, bool avail, string borrower);
};

#endif