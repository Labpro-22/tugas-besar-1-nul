#pragma once

#include <string>
#include <set>

// Valid action tile types
namespace ActionTileType {
    constexpr const char* SPESIAL = "SPESIAL";
    constexpr const char* KARTU = "KARTU";
    constexpr const char* PAJAK = "PAJAK";
    constexpr const char* FESTIVAL = "FESTIVAL";
    
    inline std::set<std::string> getValidTypes() {
        return {SPESIAL, KARTU, PAJAK, FESTIVAL};
    }
}

// Data structure for action tiles from aksi.txt
class ActionTileData {
public:
    int id = 0;                    // Position on board (1-40)
    std::string code;              // Short code (e.g., GO, DNU, FES)
    std::string name;              // Full name
    std::string type;              // SPESIAL, KARTU, PAJAK, FESTIVAL
    std::string color;             // Color tag (usually DEFAULT)
    
    // Check if this action tile is a Chance card (Kesempatan)
    bool isChanceCard() const;
    
    // Check if this action tile is a Community Chest card (Dana Umum)
    bool isCommunityChest() const;
    
    // Check if this is a tax tile
    bool isTax() const;
    
    // Check if this is a festival tile
    bool isFestival() const;
    
    // Check if this is a special tile (GO, Jail, Free Parking, Go To Jail)
    bool isSpecial() const;
    
    // Validate the data
    bool isValid() const;
};
