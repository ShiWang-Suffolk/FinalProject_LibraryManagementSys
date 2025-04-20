// core.h
#ifndef CORE_H
#define CORE_H

void closeSystem();           // Close the SQLite database

#include <sqlite3.h>

// System Initialization
void initializeSystem();

// Authentication
void loginUser();
void logoutUser();
bool isUserLoggedIn();
bool isAdmin();

// Book Interaction
void borrowBook();
void returnBook();
void searchBook();
void viewBookDetails();
void checkOverdue();
void viewBorrowingHistory();

// Information Management
void addBook();
void editBook();
void deleteBook();
void registerUser();

void fetchBookList();
void fetchBookDetailsByID(int bookID);
void fetchUserBorrowHistory(int userID);
void fetchOverdueStatus(int userID);

#endif // CORE_H
