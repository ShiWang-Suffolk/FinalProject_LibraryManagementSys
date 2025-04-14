#include "core.h"
#include <iostream>
using namespace std;


// ------------------ System Initialization ------------------

void initializeSystem() {
    // To be implemented
}

// ------------------ Authentication ------------------

void loginUser() {
    // To be implemented
}

void logoutUser() {
    // To be implemented
}

bool isUserLoggedIn() {
    // To be implemented
    return false;
}

bool isAdmin() {
    // To be implemented
    return false;
}

// ------------------ Book Interaction ------------------

void borrowBook() {
    // To be implemented
}

void returnBook() {
    // To be implemented
}

void searchBook() {
    // Calls: fetchBookList()
}

void viewBookDetails() {
    // Calls: fetchBookDetailsByID(int)
}

void checkOverdue() {
    // Calls: fetchOverdueStatus(int)
}

void viewBorrowingHistory() {
    // Calls: fetchUserBorrowHistory(int)
}

// ------------------ Info Management ------------------

void addBook() {
    // To be implemented
}


void deleteBook() {
    // To be implemented
}

void editBook() {
    // To be implemented
}


void registerUser() {
    // To be implemented
}

void fetchBookList() {
    // To be implemented
}

void fetchBookDetailsByID(int bookID) {
    // To be implemented
}

void fetchUserBorrowHistory(int userID) {
    // To be implemented
}

void fetchOverdueStatus(int userID) {
    // To be implemented
}
