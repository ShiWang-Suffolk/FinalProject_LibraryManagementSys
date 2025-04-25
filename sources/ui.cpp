// ui.cpp
#include "ui.h"
#include <iostream>
using namespace std;

void showStudentMenu() {
    cout << "\n--- Student Menu ---\n";
    cout << "0. Exit\n";  
    cout << "1. Search Book\n";
    cout << "2. View Book Details\n";
    cout << "3. Borrow Book\n";
    cout << "4. Return Book\n";
    cout << "5. View Borrow History\n";
    cout << "6. Check Overdue Status\n";
    cout << "7. View All Books\n";
    cout << "Enter choice: ";
}

void showLibrarianMenu() {
    cout << "\n--- Librarian Menu ---\n";
    cout << "0. Exit\n";
    cout << "1. Add Book\n";
    cout << "2. Edit Book\n";
    cout << "3. Delete Book\n";
    cout << "4. Register New User\n";
    cout << "Enter choice: ";
}

void showWelcomeMessage() {
    cout << "=== Welcome to the Library Management System ===\n";
}

void showExitMessage() {
    cout << "Thank you for using the system. Goodbye!\n";
}

void showInvalidChoiceMessage() {
    cout << "Invalid choice. Please try again.\n";

}

void showErrorMessage(const string& message) {
	cout << "[Error] " << message << endl;
}
















