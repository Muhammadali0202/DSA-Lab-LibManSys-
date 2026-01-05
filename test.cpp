#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include <queue>
#include <stack>
using namespace std;

/* ------------------------- BOOK STRUCT -------------------------- */

struct Book {
    int id;
    string title, author, ISBN;
    int quantity;
    int available;

    Book *left, *right;
    Book(int ID, string T, string A, string I, int Q) {
        id = ID; title = T; author = A; ISBN = I;
        quantity = Q; available = Q;
        left = right = NULL;
    }
};

/* ------------------------- BST CLASS -------------------------- */

class BookBST {
public:
    Book *root = NULL;

    Book* insert(Book* node, Book* b) {
        if (!node) return b;
        if (b->id < node->id) node->left = insert(node->left, b);
        else node->right = insert(node->right, b);
        return node;
    }

    void addBook(int id, string t, string a, string isbn, int q) {
        Book* b = new Book(id, t, a, isbn, q);
        root = insert(root, b);
    }

    Book* search(Book* node, int id) {
        if (!node) return NULL;
        if (node->id == id) return node;
        if (id < node->id) return search(node->left, id);
        return search(node->right, id);
    }

    void inorder(Book* node, vector<Book*>& list) {
        if (!node) return;
        inorder(node->left, list);
        list.push_back(node);
        inorder(node->right, list);
    }
};

/* ------------------------- STUDENT STRUCT -------------------------- */

struct BorrowNode {
    int bookID;
    BorrowNode* next;
    BorrowNode(int b) { bookID = b; next = NULL; }
};

struct Student {
    int id;
    string name;
    BorrowNode* borrowList;
    Student* next;

    Student(int i, string n) {
        id = i; name = n;
        borrowList = NULL; next = NULL;
    }

    bool canBorrow() {
        int count = 0;
        BorrowNode* t = borrowList;
        while (t) { count++; t = t->next; }
        return count < 3;
    }
};

/* ----------------------- STUDENT LIST ------------------------ */

class StudentList {
public:
    Student* head = NULL;

    void addStudent(int id, string name) {
        Student* s = new Student(id, name);
        s->next = head;
        head = s;
    }

    Student* search(int id) {
        Student* temp = head;
        while (temp) {
            if (temp->id == id) return temp;
            temp = temp->next;
        }
        return NULL;
    }
};

/* ------------------------- REQUEST QUEUE -------------------------- */

class RequestQueue {
public:
    queue<int> q;
};

/* ------------------------- HISTORY STACK -------------------------- */

struct Transaction {
    int tID, sID, bID;
    string type;
};

stack<Transaction> history;

/* ------------------------- LIBRARY MAIN CLASS -------------------------- */

class Library {
public:
    BookBST bst;
    StudentList students;
    RequestQueue rq[1000];
    int trans = 1;

    void issue(int sid, int bid) {
        Student* S = students.search(sid);
        Book* B = bst.search(bst.root, bid);

        if (!S || !B) return;
        if (B->available <= 0) {
            rq[bid].q.push(sid);
            return;
        }
        if (!S->canBorrow()) return;

        // add to student borrow list
        BorrowNode* nn = new BorrowNode(bid);
        nn->next = S->borrowList;
        S->borrowList = nn;

        B->available--;

        history.push({trans++, sid, bid, "BORROW"});
    }

    void returnBook(int sid, int bid) {
        Student* S = students.search(sid);
        Book* B = bst.search(bst.root, bid);
        if (!S || !B) return;

        BorrowNode *t = S->borrowList, *p = NULL;
        while (t && t->bookID != bid) { p = t; t = t->next; }

        if (!t) return;

        if (!p) S->borrowList = t->next;
        else p->next = t->next;

        delete t;

        B->available++;

        history.push({trans++, sid, bid, "RETURN"});

        if (!rq[bid].q.empty()) {
            int nextStudent = rq[bid].q.front();
            rq[bid].q.pop();
            issue(nextStudent, bid);
        }
    }
};

/* ------------------------- GUI VARIABLES -------------------------- */

Library lib;

// Add Book Inputs
int gui_book_id = 0;
char gui_title[100] = "";
char gui_author[100] = "";
char gui_isbn[100] = "";
int gui_qty = 0;

// Add Student Inputs
int gui_stud_id = 0;
char gui_stud_name[100] = "";

// Issue/Return Inputs
int gui_issue_sid = 0, gui_issue_bid = 0;
int gui_return_sid = 0, gui_return_bid = 0;

/* ------------------------- MAIN GUI -------------------------- */

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Full Library System (GUI + BST + DS)");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock delta;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            ImGui::SFML::ProcessEvent(ev);
            if (ev.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, delta.restart());

        ImGui::Begin("Library Management System (FULL)");

        if (ImGui::BeginTabBar("tabs")) {

            /* --------------------- ADD BOOK TAB --------------------- */
            if (ImGui::BeginTabItem("Books")) {
                ImGui::InputInt("Book ID", &gui_book_id);
                ImGui::InputText("Title", gui_title, 100);
                ImGui::InputText("Author", gui_author, 100);
                ImGui::InputText("ISBN", gui_isbn, 100);
                ImGui::InputInt("Quantity", &gui_qty);

                if (ImGui::Button("Add Book")) {
                    lib.bst.addBook(gui_book_id, gui_title, gui_author, gui_isbn, gui_qty);
                }

                ImGui::Separator();
                ImGui::Text("Books in BST:");

                vector<Book*> list;
                lib.bst.inorder(lib.bst.root, list);
                for (auto x : list) {
                    ImGui::BulletText("%d | %s | %s | Avail: %d",
                        x->id, x->title.c_str(), x->author.c_str(), x->available);
                }

                ImGui::EndTabItem();
            }

            /* --------------------- STUDENTS TAB --------------------- */
            if (ImGui::BeginTabItem("Students")) {
                ImGui::InputInt("Student ID", &gui_stud_id);
                ImGui::InputText("Student Name", gui_stud_name, 100);

                if (ImGui::Button("Register Student")) {
                    lib.students.addStudent(gui_stud_id, gui_stud_name);
                }

                ImGui::Separator();
                ImGui::Text("Student List:");

                Student* t = lib.students.head;
                while (t) {
                    ImGui::BulletText("%d | %s", t->id, t->name.c_str());
                    t = t->next;
                }

                ImGui::EndTabItem();
            }

            /* --------------------- ISSUE/RETURN TAB --------------------- */
            if (ImGui::BeginTabItem("Issue/Return")) {

                ImGui::Text("Issue Book");
                ImGui::InputInt("Student ID", &gui_issue_sid);
                ImGui::InputInt("Book ID", &gui_issue_bid);

                if (ImGui::Button("Issue")) {
                    lib.issue(gui_issue_sid, gui_issue_bid);
                }

                ImGui::Separator();

                ImGui::Text("Return Book");
                ImGui::InputInt("Student ID (R)", &gui_return_sid);
                ImGui::InputInt("Book ID (R)", &gui_return_bid);

                if (ImGui::Button("Return")) {
                    lib.returnBook(gui_return_sid, gui_return_bid);
                }

                ImGui::EndTabItem();
            }

            /* --------------------- HISTORY TAB --------------------- */
            if (ImGui::BeginTabItem("History")) {
                ImGui::Text("Recent Transactions");

                stack<Transaction> temp = history;
                int count = 0;

                while (!temp.empty() && count < 15) {
                    Transaction t = temp.top();
                    temp.pop();

                    ImGui::BulletText("TID:%d | S:%d | B:%d | %s",
                        t.tID, t.sID, t.bID, t.type.c_str());
                    count++;
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
