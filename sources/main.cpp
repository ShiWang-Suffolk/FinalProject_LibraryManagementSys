// sources/main.cpp

#include "core.h"
#include "ui.h"
#include <FL/Fl.H>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    initializeSystem();    // Initialize database & tables
    showLoginWindow();     // Show login UI
    int ret = Fl::run();   // Run FLTK event loop
    closeSystem();         // Close DB cleanly
    return ret;            // Return FLTK result
}
