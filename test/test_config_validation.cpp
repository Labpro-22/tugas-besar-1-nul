#include <iostream>
#include <string>

#include "config/Config.hpp"
#include "core/ConfigLoader.hpp"
#include "config/ConfigValidator.hpp"

void printProperty(const PropertyData& property) {
    std::cout << "  ID=" << property.id 
              << ", CODE=" << property.code
              << ", NAME=" << property.name 
              << ", TYPE=" << property.type
              << ", COLOR=" << property.color;
    
    if (property.type == "STREET") {
        std::cout << ", BUY=" << property.buyPrice
                  << ", MORTGAGE=" << property.mortgageValue
                  << ", HOUSE=" << property.houseUpgrade
                  << ", HOTEL=" << property.hotelUpgrade
                  << ", RENT=[";
        for (size_t i = 0; i < property.rentTable.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << property.rentTable[i];
        }
        std::cout << "]";
    } else {
        std::cout << ", BUY=" << property.buyPrice
                  << ", MORTGAGE=" << property.mortgageValue;
    }
    std::cout << "\n";
}

void printActionTile(const ActionTileData& tile) {
    std::cout << "  ID=" << tile.id 
              << ", CODE=" << tile.code
              << ", NAME=" << tile.name 
              << ", TYPE=" << tile.type
              << ", COLOR=" << tile.color;
    
    if (tile.isChanceCard()) {
        std::cout << " [Chance Card]";
    } else if (tile.isCommunityChest()) {
        std::cout << " [Community Chest]";
    } else if (tile.isFestival()) {
        std::cout << " [Festival]";
    } else if (tile.isTax()) {
        std::cout << " [Tax]";
    } else if (tile.isSpecial()) {
        std::cout << " [Special]";
    }
    std::cout << "\n";
}

void printIntMap(const std::map<int, int>& m, const std::string& title) {
    std::cout << "  " << title << " (" << m.size() << " entries)\n";
    for (const auto& [k, v] : m) {
        std::cout << "    " << k << " -> " << v << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::string basePath = (argc >= 2) ? argv[1] : "config";
    
    std::cout << "========================================\n";
    std::cout << "  CONFIG LOADER & VALIDATOR TEST\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Loading configuration from: " << basePath << "\n\n";
    
    try {
        // Step 1: Load configuration
        ConfigLoader loader;
        Config config = loader.loadAll(basePath);
        
        std::cout << "[STEP 1] Configuration loaded successfully!\n\n";
        
        // Display loaded data
        std::cout << "--- Properties (" << config.properties.size() << ") ---\n";
        for (const auto& p : config.properties) {
            printProperty(p);
        }
        std::cout << "\n";
        
        std::cout << "--- Action Tiles (" << config.actionTiles.size() << ") ---\n";
        for (const auto& t : config.actionTiles) {
            printActionTile(t);
        }
        std::cout << "\n";
        
        std::cout << "--- Railroad Rent Table ---\n";
        printIntMap(config.railroad.rentTable, "Rent by owned count");
        std::cout << "\n";
        
        std::cout << "--- Utility Multiplier Table ---\n";
        printIntMap(config.utility.multiplierTable, "Multiplier by owned count");
        std::cout << "\n";
        
        std::cout << "--- Tax Configuration ---\n";
        std::cout << "  PPH Flat: M" << config.tax.pphFlat << "\n";
        std::cout << "  PPH Percent: " << config.tax.pphPercent << "%\n";
        std::cout << "  PBM Flat: M" << config.tax.pbmFlat << "\n\n";
        
        std::cout << "--- Special Tiles ---\n";
        std::cout << "  GO Salary: M" << config.special.goSalary << "\n";
        std::cout << "  Jail Fine: M" << config.special.jailFine << "\n\n";
        
        std::cout << "--- Misc Settings ---\n";
        std::cout << "  Max Turns: " << config.misc.maxTurn << "\n";
        std::cout << "  Starting Balance: M" << config.misc.startingBalance << "\n\n";
        
        // Step 2: Validate configuration
        std::cout << "[STEP 2] Validating configuration...\n\n";
        
        try {
            ConfigValidator::validate(config);
            std::cout << "✅ VALIDATION PASSED!\n";
            std::cout << "   Configuration is valid and ready to use.\n\n";
            return 0;
        } catch (const ConfigValidationException& e) {
            std::cout << "❌ VALIDATION FAILED!\n\n";
            std::cout << e.what() << "\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ ERROR: " << e.what() << "\n\n";
        return 1;
    }
}
