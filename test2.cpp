#include <iostream>
#include <string>
#include <queue>
#include <stack>
using namespace std;

/* ------------------------------------------------------
                 BOOK STRUCTURE (BST)
------------------------------------------------------ */

struct Book {
    int id;
    string title;
    string author;
    string isbn;
    int quantity;
    int available;

    Book *left, *right;

    Book(int i, string t, string a, string s, int q) {
        id = i;
        title = t;
        author = a;
        isbn = s;
        quantity = q;
        available = q;
        left = right = NULL;
    }
};

/* ------------------------------------------------------
                      BOOK BST CLASS
------------------------------------------------------ */

class BookBST {
public:
    Book* root;

    BookBST() {
        root = NULL;
    }

    Book* insert(Book* node, Book* b) {
        if (!node) return b;

        if (b->id < node->id)
            node->left = insert(node->left, b);
        else
            node->right = insert(node->right, b);

        return node;
    }

    void addBook(int id, string t, string a, string s, int q) {
        Book* b = new Book(id, t, a, s, q);
        root = insert(root, b);
        cout << "\n✔ Book Added Successfully!\n";
    }

    Book* search(Book* node, int id) {
        if (!node) return NULL;
        if (id == node->id) return node;
        if (id < node->id) return search(node->left, id);
        return search(node->right, id);
    }

    void inorder(Book* node) {
        if (!node) return;
        inorder(node->left);
        cout << node->id << " | " << node->title << " | " << node->author
             << " | Qty: " << node->quantity << " | Avail: " << node->available << endl;
        inorder(node->right);
    }
};

/* ------------------------------------------------------
                STUDENT + BORROW LIST (LINKED LIST)
------------------------------------------------------ */

struct BorrowNode {
    int bookID;
    BorrowNode* next;

    BorrowNode(int id) {
        bookID = id;
        next = NULL;
    }
};

struct Student {
    int id;
    string name;
    string dept;
    BorrowNode* borrowList;
    Student* next;

    Student(int i, string n, string d) {
        id = i;
        name = n;
        dept = d;
        borrowList = NULL;
        next = NULL;
    }

    bool canBorrow() {
        int c = 0;
        BorrowNode* t = borrowList;
        while (t) {
            c++;
            t = t->next;
        }
        return c < 3;
    }
};

/* ------------------------------------------------------
                     STUDENT LIST CLASS
------------------------------------------------------ */

class StudentList {
public:
    Student* head;

    StudentList() { head = NULL; }

    void addStudent(int id, string n, string d) {
        Student* s = new Student(id, n, d);
        s->next = head;
        head = s;
        cout << "\n✔ Student Registered!\n";
    }

    Student* search(int id) {
        Student* t = head;
        while (t) {
            if (t->id == id) return t;
            t = t->next;
        }
        return NULL;
    }

    void showStudents() {
        Student* t = head;
        while (t) {
            cout << t->id << " | " << t->name << " | Dept: " << t->dept << endl;
            t = t->next;
        }
    }
};

/* ------------------------------------------------------
                 REQUEST QUEUE (PER BOOK)
------------------------------------------------------ */

class RequestQueue {
public:
    queue<int> q;
};

/* ------------------------------------------------------
                TRANSACTION HISTORY (STACK)
------------------------------------------------------ */

struct Transaction {
    int tid;
    int sid;
    int bid;
    string type;
};

stack<Transaction> history;

/* ------------------------------------------------------
                 LIBRARY SYSTEM (MAIN LOGIC)
------------------------------------------------------ */

class Library {
public:
    BookBST bst;
    StudentList students;
    RequestQueue rq[1000];
    int tidCounter = 1;

    /* ---------------- ISSUE A BOOK ---------------- */

    void issueBook(int sid, int bid) {
        Student* s = students.search(sid);
        Book* b = bst.search(bst.root, bid);

        if (!s) {
            cout << "\n❌ Student not found!\n";
            return;
        }
        if (!b) {
            cout << "\n❌ Book not found!\n";
            return;
        }

        if (!s->canBorrow()) {
            cout << "\n❌ Cannot borrow more than 3 books!\n";
            return;
        }

        if (b->available <= 0) {
            cout << "\n❌ Book unavailable. Student added to waiting queue.\n";
            rq[bid].q.push(sid);
            return;
        }

        // Add to student's borrow list
        BorrowNode* bn = new BorrowNode(bid);
        bn->next = s->borrowList;
        s->borrowList = bn;

        b->available--;

        // Add to history
        history.push({tidCounter++, sid, bid, "BORROW"});

        cout << "\n✔ Book issued successfully!\n";
    }

    /* ---------------- RETURN A BOOK ---------------- */

    void returnBook(int sid, int bid) {
        Student* s = students.search(sid);
        Book* b = bst.search(bst.root, bid);

        if (!s || !b) {
            cout << "\n❌ Invalid student ID or book ID!\n";
            return;
        }

        BorrowNode *t = s->borrowList, *p = NULL;
        while (t && t->bookID != bid) {
            p = t;
            t = t->next;
        }

        if (!t) {
            cout << "\n❌ This student didn't borrow this book!\n";
            return;
        }

        if (!p) s->borrowList = t->next;
        else p->next = t->next;

        delete t;

        b->available++;

        history.push({tidCounter++, sid, bid, "RETURN"});

        cout << "\n✔ Book returned successfully!\n";

        if (!rq[bid].q.empty()) {
            int nextStudent = rq[bid].q.front();
            rq[bid].q.pop();
            cout << "\n📌 Student " << nextStudent << " waiting — auto-issuing book.\n";
            issueBook(nextStudent, bid);
        }
    }

    /* ---------------- SHOW HISTORY ---------------- */

    void showHistory() {
        stack<Transaction> temp = history;
        cout << "\n------- LAST TRANSACTIONS --------\n";
        int count = 0;
        while (!temp.empty() && count < 15) {
            Transaction t = temp.top();
            temp.pop();
            cout << t.tid << " | SID: " << t.sid << " | BID: "
                 << t.bid << " | " << t.type << endl;
            count++;
        }
    }
};

/* ------------------------------------------------------
                         MAIN MENU
------------------------------------------------------ */

int main() {
    Library lib;
    int ch;

    while (true) {
        cout << "\n================ LIBRARY SYSTEM (CONSOLE) ================\n";
        cout << "1. Add Book\n";
        cout << "2. Show Books\n";
        cout << "3. Register Student\n";
        cout << "4. Issue Book\n";
        cout << "5. Return Book\n";
        cout << "6. Show Transaction History\n";
        cout << "7. Show Students\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> ch;

        if (ch == 0) break;

        switch (ch) {
        case 1: {
            int id, q;
            string t, a, s;

            cout << "Book ID: "; cin >> id;
            cin.ignore();
            cout << "Title: "; getline(cin, t);
            cout << "Author: "; getline(cin, a);
            cout << "ISBN: "; getline(cin, s);
            cout << "Quantity: "; cin >> q;

            lib.bst.addBook(id, t, a, s, q);
            break;
        }

        case 2:
            cout << "\n--------- BOOK LIST ---------\n";
            lib.bst.inorder(lib.bst.root);
            break;

        case 3: {
            int id;
            string n, d;

            cout << "Student ID: "; cin >> id;
            cin.ignore();
            cout << "Name: "; getline(cin, n);
            cout << "Department: "; getline(cin, d);

            lib.students.addStudent(id, n, d);
            break;
        }

        case 4: {
            int sid, bid;
            cout << "Student ID: "; cin >> sid;
            cout << "Book ID: "; cin >> bid;
            lib.issueBook(sid, bid);
            break;
        }

        case 5: {
            int sid, bid;
            cout << "Student ID: "; cin >> sid;
            cout << "Book ID: "; cin >> bid;
            lib.returnBook(sid, bid);
            break;
        }

        case 6:
            lib.showHistory();
            break;

        case 7:
            lib.students.showStudents();
            break;

        default:
            cout << "\n❌ Invalid choice!\n";
        }
    }

    cout << "\nExiting...\n";
    return 0;
}
