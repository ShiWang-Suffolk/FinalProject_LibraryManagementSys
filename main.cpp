#include "core.h"
#include "ui.h"

int main() {
    showWelcomeMessage();
    initializeSystem();

    while (!isUserLoggedIn()) {
        loginUser();
    }

    // Menu Loop
}
