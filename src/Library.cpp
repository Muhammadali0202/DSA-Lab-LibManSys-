#include "Library.h"

void Library::addBook(const Book& book)
{
    books.push_back(book);
}

bool Library::removeBook(int id)
{
    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->id == id) {
            books.erase(it);
            return true;
        }
    }
    return false;
}

Book* Library::findBook(int id)
{
    for (auto& book : books) {
        if (book.id == id)
            return &book;
    }
    return nullptr;
}

const std::vector<Book>& Library::getBooks() const
{
    return books;
}
