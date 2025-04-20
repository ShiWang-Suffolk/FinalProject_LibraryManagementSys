// main.cpp
#include "core.h"
#include "ui.h"
#include <iostream>
using namespace std;

int main() {
    showWelcomeMessage();
    initializeSystem();

    // TODO: implement real login logic
    while (!isUserLoggedIn()) {
        loginUser();
    }

    int choice;
    do {
        if (isAdmin()) {
            showLibrarianMenu();
            cin >> choice;
            switch (choice) {
                case 1: addBook();        break;    // Add new books
                case 2: editBook();       break;    // Editing books (to be implemented)
                case 3: deleteBook();     break;    // Delete books (to be implemented)
                case 4: registerUser();   break;    // Registered user (to be implemented)
                default: showInvalidChoiceMessage(); break;
            }
        } else {
            showStudentMenu();
            cin >> choice;
            switch (choice) {
                case 1: searchBook();           break;  
                case 2: viewBookDetails();      break;  // View details (to be implemented)
                case 3: borrowBook();           break;  // Borrow books (to be implemented)
                case 4: returnBook();           break;  // Return books (to be implemented)
                case 5: viewBorrowingHistory(); break;  // Borrowing history (to be implemented)
                case 6: checkOverdue();         break;  // Overdue status (to be implemented)
                default: showInvalidChoiceMessage(); break;
            }
        }
    } while (choice != 0);

    showExitMessage();
    closeSystem();  // Close DB cleanly

    return 0;
}
