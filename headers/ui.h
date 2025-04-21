// ui.h
#ifndef UI_H
#define UI_H
#include <string>

void showStudentMenu();
void showLibrarianMenu();
void showWelcomeMessage();
void showExitMessage();
void showInvalidChoiceMessage();
void showErrorMessage(const std::string& message);

#endif // UI_H
