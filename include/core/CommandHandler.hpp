#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

class GameManager;

class CommandHandler {
public:
    CommandHandler(GameManager* game);
    
    // Process a command string
    bool processCommand(const std::string& command);
    
    // Main loop for reading commands
    void runMainLoop();
    
    // Display menu
    void displayMainMenu();
    void displayHelp();
    
private:
    GameManager* game;
    bool running;
    
    // Command handlers
    void handleCetakPapan();
    void handleLemparDadu();
    void handleAturDadu(const std::vector<std::string>& args);
    void handleCetakAkta(const std::vector<std::string>& args);
    void handleCetakProperti();
    void handleGadai();
    void handleTebus();
    void handleBangun();
    void handleSimpan(const std::vector<std::string>& args);
    void handleMuat(const std::vector<std::string>& args);
    void handleCetakLog(const std::vector<std::string>& args);
    void handleGunakanKemampuan();
    void handleHelp();
    void handleExit();
    
    // Utility functions
    std::vector<std::string> tokenize(const std::string& str);
    void trim(std::string& str);
    std::string toUpper(const std::string& str);
};
