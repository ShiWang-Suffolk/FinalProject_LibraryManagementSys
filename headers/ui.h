// headers/ui.h
#ifndef UI_H
#define UI_H

#include <string>

// Basic Message Windows
void showErrorMessage(const std::string& message);
void showSuccessMessage(const std::string& message);

// GUI Windows
void showLoginWindow();
void openMenuWindow();
void openAddBookWindow();
void openEditBookWindow();
void openDeleteBookWindow();
void openSearchBookWindow();
void openViewBookDetailsWindow();
void openBorrowBookWindow();
void openReturnBookWindow();
void openViewBorrowHistoryWindow();
void openCheckOverdueWindow();
void openRegisterUserWindow();

#endif // UI_H
