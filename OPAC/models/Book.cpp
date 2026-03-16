#include "Book.h"

Book::Book() {

    id = 0;
    title = "";
    author = "";
    dewey = "";
    available = true;
    borrowedBy = "";
}

Book::Book(int i, string t, string a, string d, bool avail, string borrower) {

    id = i;
    title = t;
    author = a;
    dewey = d;
    available = avail;
    borrowedBy = borrower;
}