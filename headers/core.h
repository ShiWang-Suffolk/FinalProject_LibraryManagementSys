// headers/core.h
#ifndef CORE_H
#define CORE_H

#include <string>
#include <sqlite3.h>

// System Initialization and Closing
void initializeSystem();
void closeSystem();

// Authentication
void loginUser(const std::string& username, const std::string& password);
void logoutUser();
bool isUserLoggedIn();
bool isUserAdmin();
int getCurrentUserID();
sqlite3* getDB();
void setLoginState(bool success, int userID, bool admin);

// Book Management
bool addBook(const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity);
bool editBook(int bookID, const std::string& newTitle, const std::string& newAuthor);
bool deleteBook(int bookID);
void fetchBookList();
void fetchBookDetailsByID(int bookID);
void searchBookByKeyword(const std::string& keyword);

// Borrowing
bool borrowBook(int bookID);
bool returnBook(int bookID);
void fetchBorrowHistory(int userID);
void fetchOverdueStatus(int userID);

// User Management
bool registerUser(const std::string& name, const std::string& role, const std::string& username, const std::string& password);

#endif // CORE_H
