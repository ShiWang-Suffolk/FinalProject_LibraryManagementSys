// ui.cpp
// UI implementation for Library Management System using FLTK
// All callbacks use the two-argument signature and silence unused-parameter warnings

#include "ui.h"
#include "core.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <stdexcept>
#include <string>

struct LoginInputs {
    Fl_Input* user;
    Fl_Input* pass;
};

//--------------------------------------------------------------
// Basic pop-ups
//--------------------------------------------------------------
void showErrorMessage(const std::string& message) {
    // Display a non-modal error dialog with a single OK button
    Fl_Window* dlg = new Fl_Window(360, 120, "Error");
    Fl_Multiline_Output* out = new Fl_Multiline_Output(10, 10, 340, 60);
    out->value(message.c_str());
    Fl_Button* ok = new Fl_Button(130, 75, 100, 30, "OK");
    ok->callback([](Fl_Widget* /*w*/, void*) {
        Fl_Window* w = Fl::first_window();
        if (w) w->hide();
    });
    dlg->end();
    dlg->set_non_modal();
    dlg->show();
}

//--------------------------------------------------------------
// Login Window
//--------------------------------------------------------------


static void login_cb(Fl_Widget* w, void* data) {
    auto* inp = static_cast<LoginInputs*>(data);
    const char* uv = inp->user->value();
    const char* pv = inp->pass->value();
    std::string u = uv ? uv : "";
    std::string p = pv ? pv : "";

    loginUser(u, p);
    if (isUserLoggedIn()) {
        w->window()->hide();
        openMenuWindow();
    } else {
        showErrorMessage("Invalid username or password.");
    }
}

void showLoginWindow() {
	Fl_Window* win = new Fl_Window(360, 200, "Login");
	    auto* inp = new LoginInputs{
	        new Fl_Input(100,  30, 240, 30, "Username:"),
	        new Fl_Input(100,  80, 240, 30, "Password:")
	    };
	    inp->pass->type(FL_SECRET_INPUT);


	    Fl_Button* btn = new Fl_Button(130, 130, 100, 30, "Login");
	    btn->callback(login_cb, inp);


	    inp->user->when(FL_WHEN_ENTER_KEY);
	    inp->user->callback(login_cb, inp);

	    inp->pass->when(FL_WHEN_ENTER_KEY);
	    inp->pass->callback(login_cb, inp);

	    win->end();
	    win->show();
}

//--------------------------------------------------------------
// Main Menu
//--------------------------------------------------------------
void openMenuWindow() {
    Fl_Window* win = new Fl_Window(300, 440, "Library Menu");
    int y = 20;
    // Helper to add buttons vertically
    auto addButton = [&](const char* label, Fl_Callback cb) {
        Fl_Button* b = new Fl_Button(50, y, 200, 30, label);
        b->callback(cb, nullptr);
        y += 50;
    };
    addButton("Add Book",               [](Fl_Widget*, void*) { openAddBookWindow(); });
    addButton("Edit Book",              [](Fl_Widget*, void*) { openEditBookWindow(); });
    addButton("Delete Book",            [](Fl_Widget*, void*) { openDeleteBookWindow(); });
    addButton("Search Book",            [](Fl_Widget*, void*) { openSearchBookWindow(); });
    addButton("View Book Details",      [](Fl_Widget*, void*) { openViewBookDetailsWindow(); });
    addButton("Borrow Book",            [](Fl_Widget*, void*) { openBorrowBookWindow(); });
    addButton("Return Book",            [](Fl_Widget*, void*) { openReturnBookWindow(); });
    addButton("View My Borrow History", [](Fl_Widget*, void*) { openViewBorrowHistoryWindow(); });
    addButton("Check Overdue Items",    [](Fl_Widget*, void*) { openCheckOverdueWindow(); });
    addButton("Register New User",      [](Fl_Widget*, void*) { openRegisterUserWindow(); });
    win->end();
    win->show();
}

//--------------------------------------------------------------
// Add Book Dialog
//--------------------------------------------------------------
struct AddBookInputs {
    Fl_Input* title;
    Fl_Input* author;
    Fl_Input* isbn;
    Fl_Input* year;
    Fl_Input* quantity;
};

void openAddBookWindow() {
    Fl_Window* win = new Fl_Window(400, 350, "Add Book");
    auto* inp = new AddBookInputs {
        new Fl_Input(120, 30, 250, 30, "Title:"),
        new Fl_Input(120, 80, 250, 30, "Author:"),
        new Fl_Input(120, 130, 250, 30, "ISBN:"),
        new Fl_Input(120, 180, 250, 30, "Year:"),
        new Fl_Input(120, 230, 250, 30, "Quantity:")
    };
    Fl_Button* btn = new Fl_Button(150, 280, 100, 30, "Add");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<AddBookInputs*>(data);
        if (i->title->value()[0]=='\0' ||
            i->author->value()[0]=='\0' ||
            i->isbn->value()[0]=='\0') {
            showErrorMessage("Title, Author and ISBN cannot be empty.");
            return;
        }
        try {
            int y = std::stoi(i->year->value());
            int q = std::stoi(i->quantity->value());
            if (y < 0 || q < 0) throw std::invalid_argument("neg");
            if (addBook(i->title->value(),
                        i->author->value(),
                        i->isbn->value(),
                        y, q)) {
                w->window()->hide();
            }
        }
        catch(...) {
            showErrorMessage("Invalid year or quantity.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Edit Book Dialog
//--------------------------------------------------------------
struct EditBookInputs {
    Fl_Input* id;
    Fl_Input* newTitle;
    Fl_Input* newAuthor;
};

void openEditBookWindow() {
    Fl_Window* win = new Fl_Window(400, 250, "Edit Book");
    auto* inp = new EditBookInputs {
        new Fl_Input(120, 30, 250, 30, "Book ID:"),
        new Fl_Input(120, 80, 250, 30, "New Title:"),
        new Fl_Input(120, 130,250, 30, "New Author:")
    };
    Fl_Button* btn = new Fl_Button(150, 180, 100, 30, "Save");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<EditBookInputs*>(data);
        try {
            int bid = std::stoi(i->id->value());
            if (i->newTitle->value()[0]=='\0' || i->newAuthor->value()[0]=='\0') {
                showErrorMessage("Title and Author cannot be empty.");
                return;
            }
            if (editBook(bid, i->newTitle->value(), i->newAuthor->value())) {
                w->window()->hide();
            }
        }
        catch(...) {
            showErrorMessage("Invalid Book ID.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Delete Book Dialog
//--------------------------------------------------------------
struct DeleteBookInputs { Fl_Input* id; };

void openDeleteBookWindow() {
    Fl_Window* win = new Fl_Window(360, 180, "Delete Book");
    auto* inp = new DeleteBookInputs {
        new Fl_Input(120, 50, 200, 30, "Book ID:")
    };
    Fl_Button* btn = new Fl_Button(130, 100, 100, 30, "Delete");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<DeleteBookInputs*>(data);
        try {
            int bid = std::stoi(i->id->value());
            if (deleteBook(bid)) {
                w->window()->hide();
            }
        }
        catch(...) {
            showErrorMessage("Invalid Book ID.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Search Book Dialog
//--------------------------------------------------------------
struct SearchBookInputs { Fl_Input* keyword; };

void openSearchBookWindow() {
    Fl_Window* win = new Fl_Window(360, 180, "Search Book");
    auto* inp = new SearchBookInputs {
        new Fl_Input(120, 50, 200, 30, "Keyword:")
    };
    Fl_Button* btn = new Fl_Button(130, 100, 100, 30, "Search");
    btn->callback([](Fl_Widget* /*w*/, void* data){
        auto* i = static_cast<SearchBookInputs*>(data);
        if (i->keyword->value()[0]=='\0') {
            showErrorMessage("Enter a keyword.");
            return;
        }
        searchBookByKeyword(i->keyword->value());
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// View Book Details Dialog
//--------------------------------------------------------------
struct ViewDetailsInputs { Fl_Input* id; };

void openViewBookDetailsWindow() {
    Fl_Window* win = new Fl_Window(400, 240, "View Details");
    auto* inp = new ViewDetailsInputs {
        new Fl_Input(120, 50, 250, 30, "Book ID:")
    };
    Fl_Button* btn = new Fl_Button(150, 110, 100, 30, "Show");
    btn->callback([](Fl_Widget* /*w*/, void* data){
        auto* i = static_cast<ViewDetailsInputs*>(data);
        try {
            int bid = std::stoi(i->id->value());
            fetchBookDetailsByID(bid);
        }
        catch(...) {
            showErrorMessage("Invalid Book ID.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Borrow Book Dialog
//--------------------------------------------------------------
struct BorrowBookInputs { Fl_Input* id; };

void openBorrowBookWindow() {
    Fl_Window* win = new Fl_Window(360, 180, "Borrow Book");
    auto* inp = new BorrowBookInputs {
        new Fl_Input(120, 50, 200, 30, "Book ID:")
    };
    Fl_Button* btn = new Fl_Button(130, 100, 100, 30, "Borrow");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<BorrowBookInputs*>(data);
        try {
            int bid = std::stoi(i->id->value());
            if (borrowBook(bid)) {
                w->window()->hide();
            }
        }
        catch(...) {
            showErrorMessage("Invalid Book ID or no copies left.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Return Book Dialog
//--------------------------------------------------------------
struct ReturnBookInputs { Fl_Input* id; };

void openReturnBookWindow() {
    Fl_Window* win = new Fl_Window(360, 180, "Return Book");
    auto* inp = new ReturnBookInputs {
        new Fl_Input(120, 50, 200, 30, "Book ID:")
    };
    Fl_Button* btn = new Fl_Button(130, 100, 100, 30, "Return");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<ReturnBookInputs*>(data);
        try {
            int bid = std::stoi(i->id->value());
            if (returnBook(bid)) {
                w->window()->hide();
            }
        }
        catch(...) {
            showErrorMessage("Invalid Book ID.");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// View Borrow History Dialog
//--------------------------------------------------------------
void openViewBorrowHistoryWindow() {
    Fl_Window* win = new Fl_Window(400, 300, "My Borrow History");
    Fl_Multiline_Output* out = new Fl_Multiline_Output(10, 10, 380, 220);
    Fl_Button* btn = new Fl_Button(150, 240, 100, 30, "Close");
    fetchBorrowHistory(getCurrentUserID());
    btn->callback([](Fl_Widget* w, void*) {
        w->window()->hide();
    });
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Check Overdue Dialog
//--------------------------------------------------------------
void openCheckOverdueWindow() {
    Fl_Window* win = new Fl_Window(360, 180, "Check Overdue");
    Fl_Button* btn = new Fl_Button(130, 70, 100, 30, "Check");
    Fl_Button* closeBtn = new Fl_Button(130, 120, 100, 30, "Close");
    btn->callback([](Fl_Widget* /*w*/, void*) {
        fetchOverdueStatus(getCurrentUserID());
    });
    closeBtn->callback([](Fl_Widget* w, void*) {
        w->window()->hide();
    });
    win->end();
    win->set_non_modal();
    win->show();
}

//--------------------------------------------------------------
// Register User Dialog
//--------------------------------------------------------------
struct RegisterUserInputs {
    Fl_Input* name;
    Fl_Input* role;
    Fl_Input* username;
    Fl_Input* password;
};

void openRegisterUserWindow() {
    Fl_Window* win = new Fl_Window(400, 300, "Register User");
    auto* inp = new RegisterUserInputs {
        new Fl_Input(120, 30, 250, 30, "Name:"),
        new Fl_Input(120, 80, 250, 30, "Role (admin/student):"),
        new Fl_Input(120,130, 250, 30, "Username:"),
        new Fl_Input(120,180, 250, 30, "Password:")
    };
    inp->password->type(FL_SECRET_INPUT);
    Fl_Button* btn = new Fl_Button(150, 230, 100, 30, "Register");
    btn->callback([](Fl_Widget* w, void* data){
        auto* i = static_cast<RegisterUserInputs*>(data);
        std::string n = i->name->value();
        std::string r = i->role->value();
        std::string u = i->username->value();
        std::string p = i->password->value();
        if (n.empty() || r.empty() || u.empty() || p.empty()) {
            showErrorMessage("All fields are required.");
            return;
        }
        if (r != "admin" && r != "student") {
            showErrorMessage("Role must be 'admin' or 'student'.");
            return;
        }
        if (registerUser(n, r, u, p)) {
            w->window()->hide();
        } else {
            showErrorMessage("Registration failed (duplicate username?).");
        }
    }, inp);
    win->end();
    win->set_non_modal();
    win->show();
}
