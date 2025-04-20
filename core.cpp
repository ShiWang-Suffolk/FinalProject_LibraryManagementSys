//core.cpp
#include "core.h"
#include "ui.h"
#include <iostream>
#include <sqlite3.h>

using namespace std;

static sqlite3* db = nullptr;            // Global SQLite connection
static bool userLoggedIn = false;         // Track login status
static bool userIsAdmin  = false;         // Track user role

// ------------------ System Initialization ------------------
void initializeSystem() {
    // Open or create the database file
    if (sqlite3_open("library.db", &db) != SQLITE_OK) {
        showErrorMessage("Failed to open database: " + string(sqlite3_errmsg(db)));
        return;
    }

    // Create the books table if it doesn't exist
    const char* sql = R"SQL(
CREATE TABLE IF NOT EXISTS books (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    title    TEXT    NOT NULL,
    author   TEXT    NOT NULL,
    isbn     TEXT    UNIQUE NOT NULL,
    year     INTEGER,
    quantity INTEGER
);
)SQL";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        showErrorMessage("Table creation failed: " + string(errMsg));
        sqlite3_free(errMsg);
    }
}

// ------------------ Authentication ------------------
// Prompt for role and mark as logged in
void loginUser() {
    cout << "Select role (1=Admin, 2=Student): ";
    int r;
    cin >> r;
    userIsAdmin  = (r == 1);
    userLoggedIn = true;
}

// Return login status
bool isUserLoggedIn() {
    return userLoggedIn;
}

// Return admin flag
bool isAdmin() {
    return userIsAdmin;
}

// ------------------ Book Interaction ------------------
// Add a new book into the database
void addBook() {
    string title, author, isbn;
    int year, quantity;

    cin.ignore(); // Clear input buffer

    cout << "Enter book title (or type 'exit' to cancel): ";
    getline(cin, title);
    if (title == "exit") return;

    cout << "Enter author (or type 'exit' to cancel): ";
    getline(cin, author);
    if (author == "exit") return;

    cout << "Enter ISBN (or type 'exit' to cancel): ";
    getline(cin, isbn);
    if (isbn == "exit") return;

    cout << "Enter publication year (or type -1 to cancel): ";
    cin >> year;
    if (year == -1) return;

    cout << "Enter quantity (or type -1 to cancel): ";
    cin >> quantity;
    if (quantity == -1) return;

    // Prepare SQL insert statement
    const char* sql = "INSERT INTO books (title, author, isbn, year, quantity) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, year);
        sqlite3_bind_int(stmt, 5, quantity);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Book added successfully!" << endl;
        } else {
            showErrorMessage(sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Failed to prepare SQL statement.");
    }
}

// List all books in the database
void fetchBookList() {
    const char* sql = "SELECT id, title, author, isbn, year, quantity FROM books;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\n--- Book List ---" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt, 0)
                 << ", Title: " << sqlite3_column_text(stmt, 1)
                 << ", Author: " << sqlite3_column_text(stmt, 2)
                 << ", ISBN: " << sqlite3_column_text(stmt, 3)
                 << ", Year: " << sqlite3_column_int(stmt, 4)
                 << ", Quantity: " << sqlite3_column_int(stmt, 5)
                 << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Failed to fetch book list.");
    }
}

void borrowBook() {
    // To be implemented
}

void returnBook() {
    // To be implemented
}

void searchBook() {
    // To be implemented
}

void viewBookDetails() {
    // To be implemented
}

void checkOverdue() {
    // To be implemented
}

void viewBorrowingHistory() {
    // To be implemented
}

// ------------------ Info Management ------------------
void deleteBook() {
    // To be implemented
}

void editBook() {
    // To be implemented
}

void registerUser() {
    // To be implemented
}

void fetchBookDetailsByID(int bookID) {
    (void)bookID; // avoid unused-parameter warning
    // To be implemented
}

void fetchUserBorrowHistory(int userID) {
    (void)userID; // avoid unused-parameter warning
    // To be implemented
}

void fetchOverdueStatus(int userID) {
    (void)userID; // avoid unused-parameter warning
    // To be implemented
}

// Close the SQLite database if open
void closeSystem() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}
