#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include <vector>

class Library {
public:
    void addBook(const Book& book);
    bool removeBook(int id);
    Book* findBook(int id);
    const std::vector<Book>& getBooks() const;

private:
    std::vector<Book> books;
};

#endif
