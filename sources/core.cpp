//core.cpp
#include "core.h"
#include "ui.h"
#include <iostream>
#include <sqlite3.h>

using namespace std;

static sqlite3 *db = nullptr;            // Global SQLite connection
static bool userLoggedIn = false;         // Track login status
static bool userIsAdmin = false;         // Track user role
static int currentUserID = -1; // Store logged-in user ID         // Track user role

// ------------------ System Initialization ------------------
void initializeSystem() {
    if (sqlite3_open("library.db", &db) != SQLITE_OK) {
        showErrorMessage("Failed to open database: " + string(sqlite3_errmsg(db)));
        return;
    }

    const char* books_sql = R"SQL(
CREATE TABLE IF NOT EXISTS books (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    title    TEXT    NOT NULL,
    author   TEXT    NOT NULL,
    isbn     TEXT    UNIQUE NOT NULL,
    year     INTEGER,
    quantity INTEGER
);)SQL";

    const char* users_sql = R"SQL(
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    role TEXT CHECK(role IN ('admin','student')) NOT NULL,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
);)SQL";

    const char* loans_sql = R"SQL(
CREATE TABLE IF NOT EXISTS loans (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    book_id INTEGER NOT NULL,
    borrow_date TEXT NOT NULL,
    return_date TEXT,
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(book_id) REFERENCES books(id)
);)SQL";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, books_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        showErrorMessage("Books table creation failed: " + string(errMsg));
        sqlite3_free(errMsg);
    }
    if (sqlite3_exec(db, users_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        showErrorMessage("Users table creation failed: " + string(errMsg));
        sqlite3_free(errMsg);
    }
    if (sqlite3_exec(db, loans_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        showErrorMessage("Loans table creation failed: " + string(errMsg));
        sqlite3_free(errMsg);
    }
}

// ------------------ Authentication ------------------
// Prompt for role and mark as logged in
void loginUser() {
    string username, password;
    cin.ignore();
    cout << "Username: "; getline(cin, username);
    cout << "Password: "; getline(cin, password);

    const char* sql = "SELECT id, role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            currentUserID = sqlite3_column_int(stmt, 0);
            const char* role = (const char*)sqlite3_column_text(stmt, 1);
            userIsAdmin = (string(role) == "admin");
            userLoggedIn = true;
            cout << "Login successful. Role: " << role << endl;
        } else {
            showErrorMessage("Login failed: invalid credentials.");
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Login query failed.");
    }
}

void logoutUser() {
    userLoggedIn = false;
    userIsAdmin = false;
    currentUserID = -1;
    cout << "Logged out successfully.";
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
	if (title == "exit")
		return;

	cout << "Enter author (or type 'exit' to cancel): ";
	getline(cin, author);
	if (author == "exit")
		return;

	cout << "Enter ISBN (or type 'exit' to cancel): ";
	getline(cin, isbn);
	if (isbn == "exit")
		return;

	cout << "Enter publication year (or type -1 to cancel): ";
	cin >> year;
	if (year == -1)
		return;

	cout << "Enter quantity (or type -1 to cancel): ";
	cin >> quantity;
	if (quantity == -1)
		return;

	// Prepare SQL insert statement
	const char *sql =
			"INSERT INTO books (title, author, isbn, year, quantity) VALUES (?, ?, ?, ?, ?);";
	sqlite3_stmt *stmt;
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
	const char *sql =
			"SELECT id, title, author, isbn, year, quantity FROM books;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		cout << "\n--- Book List ---" << endl;
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			cout << "ID: " << sqlite3_column_int(stmt, 0) << ", Title: "
					<< sqlite3_column_text(stmt, 1) << ", Author: "
					<< sqlite3_column_text(stmt, 2) << ", ISBN: "
					<< sqlite3_column_text(stmt, 3) << ", Year: "
					<< sqlite3_column_int(stmt, 4) << ", Quantity: "
					<< sqlite3_column_int(stmt, 5) << endl;
		}
		sqlite3_finalize(stmt);
	} else {
		showErrorMessage("Failed to fetch book list.");
	}
}

void borrowBook() {
	int bookID;
	    if (currentUserID < 0) { showErrorMessage("You must be logged in to borrow a book."); return; }
	    cout << "Enter book ID to borrow: ";
	    cin >> bookID;

	    const char* checkSQL = "SELECT quantity FROM books WHERE id = ?;";
	    sqlite3_stmt* stmt;
	    if (sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr) == SQLITE_OK) {
	        sqlite3_bind_int(stmt, 1, bookID);
	        if (sqlite3_step(stmt) == SQLITE_ROW) {
	            int quantity = sqlite3_column_int(stmt, 0);
	            sqlite3_finalize(stmt);
	            if (quantity > 0) {
	                const char* updateSQL = "UPDATE books SET quantity = quantity - 1 WHERE id = ?;";
	                if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) == SQLITE_OK) {
	                    sqlite3_bind_int(stmt, 1, bookID);
	                    if (sqlite3_step(stmt) == SQLITE_DONE) {
	                        sqlite3_finalize(stmt);
	                        const char* insertSQL = "INSERT INTO loans (user_id, book_id, borrow_date) VALUES (?, ?, DATE('now'));";
	                        if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) == SQLITE_OK) {
	                            sqlite3_bind_int(stmt, 1, currentUserID);
	                            sqlite3_bind_int(stmt, 2, bookID);
	                            if (sqlite3_step(stmt) == SQLITE_DONE) {
	                                cout << "Book borrowed and loan recorded successfully." << endl;
	                            } else {
	                                showErrorMessage("Failed to record loan.");
	                            }
	                        }
	                        sqlite3_finalize(stmt);
	                    } else {
	                        showErrorMessage("Failed to update book quantity.");
	                    }
	                }
	            } else {
	                showErrorMessage("No copies available.");
	            }
	        } else {
	            showErrorMessage("Book not found.");
	        }
	    } else {
	        showErrorMessage("Failed to prepare check statement.");
	    }
	}


void returnBook() {
	int bookID;
	    if (currentUserID < 0) { showErrorMessage("You must be logged in to return a book."); return; }
	    cout << "Enter book ID to return: ";
	    cin >> bookID;

	    const char* updateSQL = "UPDATE books SET quantity = quantity + 1 WHERE id = ?;";
	    sqlite3_stmt* stmt;
	    if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) == SQLITE_OK) {
	        sqlite3_bind_int(stmt, 1, bookID);
	        if (sqlite3_step(stmt) == SQLITE_DONE) {
	            sqlite3_finalize(stmt);
	            const char* returnSQL = R"SQL(
	                UPDATE loans
	                SET return_date = DATE('now')
	                WHERE user_id = ? AND book_id = ? AND return_date IS NULL
	                ORDER BY borrow_date DESC LIMIT 1;
	            )SQL";
	            if (sqlite3_prepare_v2(db, returnSQL, -1, &stmt, nullptr) == SQLITE_OK) {
	                sqlite3_bind_int(stmt, 1, currentUserID);
	                sqlite3_bind_int(stmt, 2, bookID);
	                if (sqlite3_step(stmt) == SQLITE_DONE) {
	                    cout << "Book returned and loan updated successfully." << endl;
	                } else {
	                    showErrorMessage("Failed to update loan return date.");
	                }
	                sqlite3_finalize(stmt);
	            }
	        } else {
	            showErrorMessage("Failed to update book quantity.");
	        }
	    } else {
	        showErrorMessage("Failed to prepare return statement.");
	    }
}

void searchBook() {
	cin.ignore();
	cout << "Enter keyword to search (title or author): ";
	string keyword;
	getline(cin, keyword);

	string query =
			"SELECT id, title, author FROM books WHERE title LIKE ? OR author LIKE ?;";
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		string pattern = "%" + keyword + "%";
		sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_STATIC);

		cout << "\n--- Search Results ---" << endl;
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			cout << "ID: " << sqlite3_column_int(stmt, 0) << ", Title: "
					<< sqlite3_column_text(stmt, 1) << ", Author: "
					<< sqlite3_column_text(stmt, 2) << endl;
		}
		sqlite3_finalize(stmt);
	} else {
		showErrorMessage("Search failed.");
	}
}

void viewBookDetails() {
	int bookID;
	cout << "Enter book ID to view: ";
	cin >> bookID;
	fetchBookDetailsByID(bookID);
}

void checkOverdue() {
	if (currentUserID < 0) { showErrorMessage("You must be logged in to check overdue status."); return; }
	    int userID = currentUserID;
	    fetchOverdueStatus(userID);
	}

void viewBorrowingHistory() {
	if (currentUserID < 0) { showErrorMessage("You must be logged in to view history."); return; }
	    int userID = currentUserID;
	    fetchUserBorrowHistory(userID);
}

// ------------------ Info Management ------------------
void deleteBook() {
	int bookID;
	cout << "Enter book ID to delete: ";
	cin >> bookID;

	const char *sql = "DELETE FROM books WHERE id = ?;";
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, bookID);
		if (sqlite3_step(stmt) == SQLITE_DONE) {
			cout << "Book deleted successfully." << endl;
		} else {
			showErrorMessage("Delete failed.");
		}
		sqlite3_finalize(stmt);
	} else {
		showErrorMessage("Failed to prepare delete statement.");
	}
}

void editBook() {
	int bookID;
	cin.ignore();
	cout << "Enter book ID to edit: ";
	cin >> bookID;
	cin.ignore();

	string newTitle, newAuthor;
	cout << "Enter new title: ";
	getline(cin, newTitle);
	cout << "Enter new author: ";
	getline(cin, newAuthor);

	const char *sql = "UPDATE books SET title = ?, author = ? WHERE id = ?;";
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, newTitle.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, newAuthor.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 3, bookID);
		if (sqlite3_step(stmt) == SQLITE_DONE) {
			cout << "Book updated successfully." << endl;
		} else {
			showErrorMessage("Update failed.");
		}
		sqlite3_finalize(stmt);
	} else {
		showErrorMessage("Failed to prepare update statement.");
	}
}

void registerUser() {
    string name, role, username, password;
    cin.ignore();
    cout << "Enter name: "; getline(cin, name);
    cout << "Enter role (admin/student): "; getline(cin, role);
    cout << "Enter username: "; getline(cin, username);
    cout << "Enter password: "; getline(cin, password);

    const char* sql = "INSERT INTO users (name, role, username, password) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "User registered successfully." << endl;
        } else {
            showErrorMessage("User registration failed.");
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Failed to prepare registration statement.");
    }
}


void fetchBookDetailsByID(int bookID) {
	const char *sql =
			"SELECT title, author, isbn, year, quantity FROM books WHERE id = ?;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, bookID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			cout << "\n--- Book Details ---\n";
			cout << "Title: " << sqlite3_column_text(stmt, 0) << endl;
			cout << "Author: " << sqlite3_column_text(stmt, 1) << endl;
			cout << "ISBN: " << sqlite3_column_text(stmt, 2) << endl;
			cout << "Year: " << sqlite3_column_int(stmt, 3) << endl;
			cout << "Quantity: " << sqlite3_column_int(stmt, 4) << endl;
		} else {
			showErrorMessage("Book ID not found.");
		}
		sqlite3_finalize(stmt);
	} else {
		showErrorMessage("Failed to prepare detail query.");
	}
}

void fetchUserBorrowHistory(int userID) {
    const char* sql = R"SQL(
SELECT b.title, l.borrow_date, l.return_date
FROM loans l
JOIN books b ON l.book_id = b.id
WHERE l.user_id = ?;
)SQL";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userID);
        cout << "\n--- Borrow History ---\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Title: " << sqlite3_column_text(stmt, 0)
                 << ", Borrowed: " << sqlite3_column_text(stmt, 1)
                 << ", Returned: " << (sqlite3_column_type(stmt, 2) == SQLITE_NULL ? "Not returned" : (const char*)sqlite3_column_text(stmt, 2))
                 << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Failed to fetch borrow history.");
    }
}


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
                cout << "You have " << count << " overdue item(s)." << endl;
            } else {
                cout << "No overdue items." << endl;
            }
        }
        sqlite3_finalize(stmt);
    } else {
        showErrorMessage("Failed to check overdue status.");
    }
}


// Close the SQLite database if open
void closeSystem() {
	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}
}
