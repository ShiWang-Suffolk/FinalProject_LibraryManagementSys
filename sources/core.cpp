// sources/core.cpp

#include "core.h"
#include "ui.h"
#include <iostream>
#include <stdexcept>
#include <sqlite3.h>

using namespace std;

// ----------------------------------------------------------------
// RAII wrapper for sqlite3_stmt: ensures sqlite3_finalize is called
// ----------------------------------------------------------------
struct Stmt {
    sqlite3_stmt* stmt;
    // Prepare SQL statement or throw on error
    Stmt(sqlite3* db, const char* sql) {
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw runtime_error(sqlite3_errmsg(db));
        }
    }
    // Finalize when leaving scope
    ~Stmt() {
        sqlite3_finalize(stmt);
    }
};

// ----------------------------------------------------------------
// Global state for DB handle and current user
// ----------------------------------------------------------------
static sqlite3* db            = nullptr;
static bool     userLoggedIn  = false;
static bool     userIsAdmin   = false;
static int      currentUserID = -1;

// ----------------------------------------------------------------
// Open (or create) library.db and its tables
// ----------------------------------------------------------------
void initializeSystem() {
    if (sqlite3_open("library.db", &db) != SQLITE_OK) {
        showErrorMessage("Failed to open database: " + string(sqlite3_errmsg(db)));
        return;
    }
    // Create books table
    const char* books_sql = R"SQL(
        CREATE TABLE IF NOT EXISTS books (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            title    TEXT    NOT NULL,
            author   TEXT    NOT NULL,
            isbn     TEXT    UNIQUE NOT NULL,
            year     INTEGER,
            quantity INTEGER
        );
    )SQL";
    sqlite3_exec(db, books_sql, nullptr, nullptr, nullptr);

    // Create users table
    const char* users_sql = R"SQL(
        CREATE TABLE IF NOT EXISTS users (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            name     TEXT    NOT NULL,
            role     TEXT    CHECK(role IN ('admin','student')) NOT NULL,
            username TEXT    UNIQUE NOT NULL,
            password TEXT    NOT NULL
        );
    )SQL";
    sqlite3_exec(db, users_sql, nullptr, nullptr, nullptr);

    // Create loans table
    const char* loans_sql = R"SQL(
        CREATE TABLE IF NOT EXISTS loans (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id     INTEGER NOT NULL,
            book_id     INTEGER NOT NULL,
            borrow_date TEXT    NOT NULL,
            return_date TEXT,
            FOREIGN KEY(user_id) REFERENCES users(id),
            FOREIGN KEY(book_id) REFERENCES books(id)
        );
    )SQL";
    sqlite3_exec(db, loans_sql, nullptr, nullptr, nullptr);
}

// ----------------------------------------------------------------
// Close the SQLite database when the program exits
// ----------------------------------------------------------------
void closeSystem() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

// ----------------------------------------------------------------
// Attempt login: if success, set global state and show message
// ----------------------------------------------------------------
void loginUser(const string& username, const string& password) {
	 const char* sql = "SELECT id, role FROM users WHERE username = ? AND password = ?;";
	    sqlite3_stmt* stmt;

	    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
	        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	        if (sqlite3_step(stmt) == SQLITE_ROW) {

	            currentUserID = sqlite3_column_int(stmt, 0);
	            string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
	            userIsAdmin = (role == "admin");
	            userLoggedIn = true;
	            // showSuccessMessage("Login successful.");
	        } else {

	            showErrorMessage("Login failed: Invalid credentials.");
	        }
	        sqlite3_finalize(stmt);
	    } else {

	        showErrorMessage("Failed to prepare login statement.");
	    }
}

// ----------------------------------------------------------------
// Log out current user
// ----------------------------------------------------------------
void logoutUser() {
    userLoggedIn  = false;
    userIsAdmin   = false;
    currentUserID = -1;
}

// ----------------------------------------------------------------
// Query current login/admin state
// ----------------------------------------------------------------
bool isUserLoggedIn() { return userLoggedIn; }
bool isUserAdmin()    { return userIsAdmin;    }
int  getCurrentUserID(){ return currentUserID;  }
sqlite3* getDB()      { return db;             }

// ----------------------------------------------------------------
// Set login state manually (for testing)
// ----------------------------------------------------------------
void setLoginState(bool success, int userID, bool admin) {
    userLoggedIn  = success;
    currentUserID = userID;
    userIsAdmin   = admin;
}

// ----------------------------------------------------------------
// Add a new book record
// ----------------------------------------------------------------
bool addBook(const string& title, const string& author,
             const string& isbn,  int year,     int quantity)
{
    const char* sql = "INSERT INTO books(title,author,isbn,year,quantity)"
                      " VALUES(?,?,?,?,?);";
    try {
        Stmt stmt(db, sql);
        sqlite3_bind_text(stmt.stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt, 2, author.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt.stmt, 4, year);
        sqlite3_bind_int(stmt.stmt, 5, quantity);
        if (sqlite3_step(stmt.stmt) != SQLITE_DONE) {
            throw runtime_error(sqlite3_errmsg(db));
        }
        return true;
    } catch (const exception& ex) {
        showErrorMessage(string("Add book failed: ") + ex.what());
        return false;
    }
}

// ----------------------------------------------------------------
// Edit an existing book's title/author
// ----------------------------------------------------------------
bool editBook(int bookID, const string& newTitle, const string& newAuthor) {
    const char* sql = "UPDATE books SET title=?,author=? WHERE id=?;";
    try {
        Stmt stmt(db, sql);
        sqlite3_bind_text(stmt.stmt, 1, newTitle.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt, 2, newAuthor.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt.stmt, 3, bookID);
        if (sqlite3_step(stmt.stmt) != SQLITE_DONE) {
            throw runtime_error(sqlite3_errmsg(db));
        }
        return true;
    } catch (const exception& ex) {
        showErrorMessage(string("Edit book failed: ") + ex.what());
        return false;
    }
}

// ----------------------------------------------------------------
// Delete a book by ID
// ----------------------------------------------------------------
bool deleteBook(int bookID) {
    const char* sql = "DELETE FROM books WHERE id=?;";
    try {
        Stmt stmt(db, sql);
        sqlite3_bind_int(stmt.stmt, 1, bookID);
        if (sqlite3_step(stmt.stmt) != SQLITE_DONE) {
            throw runtime_error(sqlite3_errmsg(db));
        }
        return true;
    } catch (const exception& ex) {
        showErrorMessage(string("Delete book failed: ") + ex.what());
        return false;
    }
}

// ----------------------------------------------------------------
// List all books on console
// ----------------------------------------------------------------
void fetchBookList() {
    const char* sql = "SELECT id,title,author FROM books;";
    try {
        Stmt stmt(db, sql);
        while (sqlite3_step(stmt.stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt.stmt,0)
                 << ", Title: " << sqlite3_column_text(stmt.stmt,1)
                 << ", Author: " << sqlite3_column_text(stmt.stmt,2)
                 << endl;
        }
    } catch (...) {
        showErrorMessage("Failed to fetch book list.");
    }
}

// ----------------------------------------------------------------
// Show detailed info for one book
// ----------------------------------------------------------------
void fetchBookDetailsByID(int bookID) {
    const char* sql = "SELECT title,author,isbn,year,quantity FROM books WHERE id=?;";
    try {
        Stmt stmt(db, sql);
        sqlite3_bind_int(stmt.stmt,1,bookID);
        if (sqlite3_step(stmt.stmt) == SQLITE_ROW) {
            cout << "Title: "    << sqlite3_column_text(stmt.stmt,0) << endl;
            cout << "Author: "   << sqlite3_column_text(stmt.stmt,1) << endl;
            cout << "ISBN: "     << sqlite3_column_text(stmt.stmt,2) << endl;
            cout << "Year: "     << sqlite3_column_int(stmt.stmt,3)  << endl;
            cout << "Quantity: " << sqlite3_column_int(stmt.stmt,4)  << endl;
        } else {
            showErrorMessage("Book not found.");
        }
    } catch (...) {
        showErrorMessage("Failed to fetch book details.");
    }
}

// ----------------------------------------------------------------
// Search books by title or author keyword
// ----------------------------------------------------------------
void searchBookByKeyword(const string& keyword) {
    const char* sql = "SELECT id,title,author FROM books "
                      "WHERE title LIKE ? OR author LIKE ?;";
    try {
        Stmt stmt(db, sql);
        string pat = "%" + keyword + "%";
        sqlite3_bind_text(stmt.stmt,1,pat.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt,2,pat.c_str(),-1,SQLITE_STATIC);
        while (sqlite3_step(stmt.stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt.stmt,0)
                 << ", Title: " << sqlite3_column_text(stmt.stmt,1)
                 << ", Author: " << sqlite3_column_text(stmt.stmt,2)
                 << endl;
        }
    } catch (...) {
        showErrorMessage("Search failed.");
    }
}

// ----------------------------------------------------------------
// Borrow a book within a transaction: decrement qty + insert loan
// ----------------------------------------------------------------
bool borrowBook(int bookID) {
    if (!isUserLoggedIn()) {
        showErrorMessage("You must be logged in to borrow.");
        return false;
    }
    char* err = nullptr;
    if (sqlite3_exec(db,"BEGIN;",nullptr,nullptr,&err)!=SQLITE_OK) {
        showErrorMessage(string("Begin failed: ")+err);
        sqlite3_free(err);
        return false;
    }
    try {
        // Decrement quantity
        Stmt s1(db,"UPDATE books SET quantity=quantity-1 WHERE id=? AND quantity>0;");
        sqlite3_bind_int(s1.stmt,1,bookID);
        if (sqlite3_step(s1.stmt)!=SQLITE_DONE)
            throw runtime_error("No copies available.");

        // Insert loan record
        Stmt s2(db,"INSERT INTO loans(user_id,book_id,borrow_date) VALUES(?,?,DATE('now'));");
        sqlite3_bind_int(s2.stmt,1,currentUserID);
        sqlite3_bind_int(s2.stmt,2,bookID);
        if (sqlite3_step(s2.stmt)!=SQLITE_DONE)
            throw runtime_error(sqlite3_errmsg(db));

        // Commit
        if (sqlite3_exec(db,"COMMIT;",nullptr,nullptr,&err)!=SQLITE_OK)
            throw runtime_error(err);

        return true;
    }
    catch (const exception& ex) {
        sqlite3_exec(db,"ROLLBACK;",nullptr,nullptr,nullptr);
        showErrorMessage(string("Borrow failed: ") + ex.what());
        return false;
    }
}

// ----------------------------------------------------------------
// Return a book within a transaction: increment qty + update loan
// ----------------------------------------------------------------
bool returnBook(int bookID) {
    if (!isUserLoggedIn()) {
        showErrorMessage("You must be logged in to return.");
        return false;
    }
    char* err = nullptr;
    if (sqlite3_exec(db,"BEGIN;",nullptr,nullptr,&err)!=SQLITE_OK) {
        showErrorMessage(string("Begin failed: ")+err);
        sqlite3_free(err);
        return false;
    }
    try {
        // Increment quantity
        Stmt s1(db,"UPDATE books SET quantity=quantity+1 WHERE id=?;");
        sqlite3_bind_int(s1.stmt,1,bookID);
        if (sqlite3_step(s1.stmt)!=SQLITE_DONE)
            throw runtime_error(sqlite3_errmsg(db));

        // Update return_date in latest loan
        const char* upSQL = R"SQL(
            UPDATE loans SET return_date=DATE('now')
             WHERE user_id=? AND book_id=? AND return_date IS NULL
             ORDER BY borrow_date DESC LIMIT 1;
        )SQL";
        Stmt s2(db,upSQL);
        sqlite3_bind_int(s2.stmt,1,currentUserID);
        sqlite3_bind_int(s2.stmt,2,bookID);
        sqlite3_step(s2.stmt);

        // Commit
        if (sqlite3_exec(db,"COMMIT;",nullptr,nullptr,&err)!=SQLITE_OK)
            throw runtime_error(err);

        return true;
    }
    catch (const exception& ex) {
        sqlite3_exec(db,"ROLLBACK;",nullptr,nullptr,nullptr);
        showErrorMessage(string("Return failed: ") + ex.what());
        return false;
    }
}

// ----------------------------------------------------------------
// Fetch borrow history for a user
// ----------------------------------------------------------------
void fetchBorrowHistory(int userID) {
    const char* sql = R"SQL(
        SELECT b.title,l.borrow_date,l.return_date
          FROM loans l
          JOIN books b ON l.book_id=b.id
         WHERE l.user_id=?;
    )SQL";
    try {
        Stmt stmt(db,sql);
        sqlite3_bind_int(stmt.stmt,1,userID);
        while (sqlite3_step(stmt.stmt)==SQLITE_ROW) {
            cout<<"Title: "<<sqlite3_column_text(stmt.stmt,0)
                <<", Borrowed: "<<sqlite3_column_text(stmt.stmt,1)
                <<", Returned: "<<(sqlite3_column_type(stmt.stmt,2)==SQLITE_NULL?"Not yet":(const char*)sqlite3_column_text(stmt.stmt,2))
                <<endl;
        }
    } catch (...) {
        showErrorMessage("Failed to fetch history.");
    }
}

// ----------------------------------------------------------------
// Show overdue count for a user
// ----------------------------------------------------------------
void fetchOverdueStatus(int userID) {
	const char* sql = R"SQL(
	        SELECT COUNT(*) FROM loans
	        WHERE user_id = ? AND return_date IS NULL AND DATE(borrow_date, '+14 days') < DATE('now');
	    )SQL";
	    sqlite3_stmt* stmt;
	    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
	        sqlite3_bind_int(stmt, 1, userID);

	        if (sqlite3_step(stmt) == SQLITE_ROW) {
	            int count = sqlite3_column_int(stmt, 0);
	            if (count > 0) {

	                showErrorMessage("You have " + to_string(count) + " overdue items.");
	            }

	        }
	        sqlite3_finalize(stmt);
	    }
}

// ----------------------------------------------------------------
// Register a new user with name, role, username, password
// ----------------------------------------------------------------
bool registerUser(const string& name, const string& role,
                  const string& username, const string& password)
{
    const char* sql = "INSERT INTO users(name,role,username,password) VALUES(?,?,?,?);";
    try {
        Stmt stmt(db,sql);
        sqlite3_bind_text(stmt.stmt,1,name.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt,2,role.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt,3,username.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt.stmt,4,password.c_str(),-1,SQLITE_STATIC);
        if (sqlite3_step(stmt.stmt)!=SQLITE_DONE)
            throw runtime_error(sqlite3_errmsg(db));
        return true;
    } catch (const exception& ex) {
        showErrorMessage(string("User registration failed: ")+ex.what());
        return false;
    }
}
