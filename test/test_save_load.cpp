#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

#include "player/Player.h"

#include "config/Config.hpp"
#include "core/ConfigLoader.hpp"
#include "config/ConfigValidator.hpp"
#include "core/GameManager.hpp"

namespace fs = std::filesystem;

void displayBanner() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║   NIMONSPOLI - SAVE/LOAD TEST                                ║\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════════════╝\n";
}

void testSaveLoad() {
    std::cout << "\n========== TEST SAVE/LOAD ==========\n\n";
    
    // Setup config
    Config config;
    config.misc.maxTurn = 15;
    config.misc.startingBalance = 1000;
    config.special.goSalary = 200;
    config.special.jailFine = 50;
    config.tax.pphFlat = 150;
    config.tax.pphPercent = 10;
    config.tax.pbmFlat = 200;
    
    // Create game
    GameManager game;
    game.initialize(config);
    
    std::vector<std::string> playerNames = {"Alice", "Bob", "Charlie"};
    game.setupNewGame(playerNames.size(), playerNames);
    game.startGame();
    
    std::cout << "Game created with " << playerNames.size() << " players\n";
    std::cout << "Current turn: " << game.getCurrentTurn() << "/" << game.getMaxTurn() << "\n";
    if (game.getCurrentPlayer()) {
        std::cout << "Active player: " << game.getCurrentPlayer()->getUsername() << "\n";
    }
    
    // Test save
    std::string saveFile = "test_save.nmp";
    std::cout << "\n--- Testing SAVE ---\n";
    game.saveGame(saveFile);
    
    // Check if file was created
    if (fs::exists(saveFile)) {
        std::cout << "✅ Save file created successfully: " << saveFile << "\n";
        std::cout << "   File size: " << fs::file_size(saveFile) << " bytes\n";
        
        // Display file contents
        std::cout << "\n--- Save File Contents ---\n";
        std::ifstream file(saveFile);
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line) && lineNum < 20) {
            std::cout << lineNum + 1 << ": " << line << "\n";
            lineNum++;
        }
        if (lineNum >= 20) {
            std::cout << "... (truncated)\n";
        }
        file.close();
    } else {
        std::cout << "❌ Save file was not created\n";
    }
    
    // Test load
    std::cout << "\n--- Testing LOAD ---\n";
    GameManager loadedGame;
    loadedGame.initialize(config);
    loadedGame.setupNewGame(playerNames.size(), playerNames);
    loadedGame.loadGame(saveFile);
    
    std::cout << "\nLoaded game state:\n";
    std::cout << "Current turn: " << loadedGame.getCurrentTurn() << "/" << loadedGame.getMaxTurn() << "\n";
    if (loadedGame.getCurrentPlayer()) {
        std::cout << "Active player: " << loadedGame.getCurrentPlayer()->getUsername() << "\n";
    }
    
    // Cleanup
    if (fs::exists(saveFile)) {
        fs::remove(saveFile);
        std::cout << "\n✅ Test save file cleaned up\n";
    }
    
    std::cout << "\n========== SAVE/LOAD TEST COMPLETE ==========\n";
}

int main() {
    displayBanner();
    
    try {
        testSaveLoad();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
}
