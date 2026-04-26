#include <iostream>
#include <string>

#include "core/GameEngine.hpp"

int main(int argc, char* argv[]) {
    GameEngine ge(10);
    
    // Check for command line arguments
    if (argc >= 2) {
        std::string mode = argv[1];
        if (mode == "--gui" || mode == "-g") {
            std::cout << "Starting Nimonspoli in GUI mode...\n";
            ge.runGUI();
            return 0;
        } else if (mode == "--console" || mode == "-c") {
            std::cout << "Starting Nimonspoli in Console mode...\n";
            ge.run();
            return 0;
        }
    }
    
    // Interactive mode selection
    std::cout << "\n========================================\n";
    std::cout << "         N I M O N S P O L I\n";
    std::cout << "========================================\n";
    std::cout << "Select game mode:\n";
    std::cout << "  1. GUI Mode    - Play with visual board\n";
    std::cout << "  2. Console Mode - Play in terminal\n";
    std::cout << "\nChoice (1-2): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1" || choice == "gui" || choice == "GUI") {
        std::cout << "\nStarting GUI mode...\n";
        ge.runGUI();
    } else {
        std::cout << "\nStarting Console mode...\n";
        ge.run();
    }
    
    return 0;
}
