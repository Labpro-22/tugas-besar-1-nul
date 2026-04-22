#include "config/ActionTileData.hpp"

bool ActionTileData::isChanceCard() const {
    return type == ActionTileType::KARTU && code == "KSP";
}

bool ActionTileData::isCommunityChest() const {
    return type == ActionTileType::KARTU && code == "DNU";
}

bool ActionTileData::isTax() const {
    return type == ActionTileType::PAJAK;
}

bool ActionTileData::isFestival() const {
    return type == ActionTileType::FESTIVAL;
}

bool ActionTileData::isSpecial() const {
    return type == ActionTileType::SPESIAL;
}

bool ActionTileData::isValid() const {
    // ID must be between 1 and 40
    if (id < 1 || id > 40) {
        return false;
    }
    
    // Code cannot be empty
    if (code.empty()) {
        return false;
    }
    
    // Type must be valid
    auto validTypes = ActionTileType::getValidTypes();
    if (validTypes.find(type) == validTypes.end()) {
        return false;
    }
    
    return true;
}
