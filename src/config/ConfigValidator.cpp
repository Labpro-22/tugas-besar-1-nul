#include "config/ConfigValidator.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>

// ============================================================================
// Validation Constants
// ============================================================================

const std::set<std::string> ConfigValidator::VALID_PROPERTY_TYPES = {
    "STREET", "RAILROAD", "UTILITY"
};

const std::set<std::string> ConfigValidator::VALID_COLORS = {
    "COKLAT", "BIRU_MUDA", "MERAH_MUDA", "ORANGE", 
    "MERAH", "KUNING", "HIJAU", "BIRU_TUA", 
    "DEFAULT", "ABU_ABU"
};

const std::set<std::string> ConfigValidator::VALID_ACTION_TYPES = {
    "SPESIAL", "KARTU", "PAJAK", "FESTIVAL"
};

// ============================================================================
// ConfigValidationError Implementation
// ============================================================================

std::string ConfigValidationError::toString() const {
    std::ostringstream oss;
    oss << file << ":" << lineNumber;
    if (!field.empty()) {
        oss << " [" << field << "]";
    }
    oss << " " << message;
    if (!value.empty()) {
        oss << " (got: '" << value << "')";
    }
    if (!suggestion.empty()) {
        oss << "\n  Suggestion: " << suggestion;
    }
    return oss.str();
}

// ============================================================================
// ConfigValidationException Implementation
// ============================================================================

ConfigValidationException::ConfigValidationException(
    std::vector<ConfigValidationError> errors)
    : errors_(std::move(errors)) {}

const char* ConfigValidationException::what() const noexcept {
    if (errors_.empty()) {
        message_ = "Configuration validation failed";
        return message_.c_str();
    }
    
    std::ostringstream oss;
    oss << "Configuration validation failed with " << errors_.size() 
        << " error(s):\n\n";
    
    for (size_t i = 0; i < errors_.size(); ++i) {
        oss << (i + 1) << ". " << errors_[i].toString() << "\n\n";
    }
    
    message_ = oss.str();
    return message_.c_str();
}

// ============================================================================
// Main Validation Method
// ============================================================================

void ConfigValidator::validate(const Config& config) {
    std::vector<ConfigValidationError> errors;
    
    // Level 1 & 2: Syntax and Semantic Validation
    validateProperties(config, errors);
    validateActionTiles(config, errors);
    validateRailroad(config, errors);
    validateUtility(config, errors);
    validateTax(config, errors);
    validateSpecial(config, errors);
    validateMisc(config, errors);
    
    // Level 3 & 4: Business Rules and Cross-File Validation
    validateCrossReferences(config, errors);
    
    if (!errors.empty()) {
        throw ConfigValidationException(std::move(errors));
    }
}

// ============================================================================
// Property Validation
// ============================================================================

void ConfigValidator::validateProperties(
    const Config& config, 
    std::vector<ConfigValidationError>& errors) {
    
    const auto& properties = config.properties;
    
    validatePropertyCounts(properties, errors);
    validatePropertyIds(properties, errors);
    validatePropertyCodes(properties, errors);
    validatePropertyTypes(properties, errors);
    validatePropertyColors(properties, errors);
    validatePropertyPrices(properties, errors);
    validateRentTable(properties, errors);
    validateColorGroups(properties, errors);
}

void ConfigValidator::validatePropertyCounts(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    if (properties.size() != static_cast<size_t>(EXPECTED_PROPERTY_COUNT)) {
        addError(errors, "property.txt", 0, "", 
                 std::to_string(properties.size()),
                 "Invalid property count",
                 "Expected " + std::to_string(EXPECTED_PROPERTY_COUNT) + 
                 " properties (22 STREET + 4 RAILROAD + 2 UTILITY), got " +
                 std::to_string(properties.size()));
    }
}

void ConfigValidator::validatePropertyIds(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    std::set<int> usedIds;
    
    for (const auto& prop : properties) {
        // Check range
        if (prop.id < MIN_PROPERTY_ID || prop.id > MAX_PROPERTY_ID) {
            addError(errors, "property.txt", prop.id, "ID",
                     std::to_string(prop.id),
                     "Property ID out of range",
                     "ID must be between " + std::to_string(MIN_PROPERTY_ID) + 
                     " and " + std::to_string(MAX_PROPERTY_ID));
        }
        
        // Check uniqueness
        if (!usedIds.insert(prop.id).second) {
            addError(errors, "property.txt", prop.id, "ID",
                     std::to_string(prop.id),
                     "Duplicate property ID",
                     "Each property must have a unique ID");
        }
    }
}

void ConfigValidator::validatePropertyCodes(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    std::set<std::string> usedCodes;
    
    for (const auto& prop : properties) {
        // Check empty
        if (prop.code.empty()) {
            addError(errors, "property.txt", prop.id, "CODE", "",
                     "Property code cannot be empty",
                     "Provide a unique code (e.g., GRT, JKT)");
            continue;
        }
        
        // Check length (max 3 chars based on spec)
        if (prop.code.length() > 3) {
            addError(errors, "property.txt", prop.id, "CODE", prop.code,
                     "Property code too long",
                     "Code should be 3 characters or less (e.g., GRT, JKT)");
        }
        
        // Check uniqueness
        if (!usedCodes.insert(prop.code).second) {
            addError(errors, "property.txt", prop.id, "CODE", prop.code,
                     "Duplicate property code",
                     "Each property must have a unique code");
        }
    }
}

void ConfigValidator::validatePropertyTypes(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    for (const auto& prop : properties) {
        if (VALID_PROPERTY_TYPES.find(prop.type) == VALID_PROPERTY_TYPES.end()) {
            addError(errors, "property.txt", prop.id, "TYPE", prop.type,
                     "Invalid property type",
                     "Valid types are: STREET, RAILROAD, UTILITY");
        }
    }
}

void ConfigValidator::validatePropertyColors(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    for (const auto& prop : properties) {
        if (VALID_COLORS.find(prop.color) == VALID_COLORS.end()) {
            addError(errors, "property.txt", prop.id, "COLOR", prop.color,
                     "Invalid color",
                     "Valid colors are: COKLAT, BIRU_MUDA, MERAH_MUDA, ORANGE, "
                     "MERAH, KUNING, HIJAU, BIRU_TUA, DEFAULT, ABU_ABU");
        }
        
        // RAILROAD and UTILITY should have DEFAULT or ABU_ABU color
        if (prop.type == "RAILROAD" && prop.color != "DEFAULT") {
            addError(errors, "property.txt", prop.id, "COLOR", prop.color,
                     "Railroad should have DEFAULT color",
                     "Change color to DEFAULT");
        }
        if (prop.type == "UTILITY" && prop.color != "ABU_ABU") {
            addError(errors, "property.txt", prop.id, "COLOR", prop.color,
                     "Utility should have ABU_ABU color",
                     "Change color to ABU_ABU");
        }
    }
}

void ConfigValidator::validatePropertyPrices(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    for (const auto& prop : properties) {
        // All prices must be non-negative
        if (prop.buyPrice < 0) {
            addError(errors, "property.txt", prop.id, "HARGA_LAHAN",
                     std::to_string(prop.buyPrice),
                     "Buy price cannot be negative",
                     "Provide a non-negative value");
        }
        
        if (prop.mortgageValue < 0) {
            addError(errors, "property.txt", prop.id, "NILAI_GADAI",
                     std::to_string(prop.mortgageValue),
                     "Mortgage value cannot be negative",
                     "Provide a non-negative value");
        }
        
        // STREET properties must have positive prices
        if (prop.type == "STREET") {
            if (prop.buyPrice <= 0) {
                addError(errors, "property.txt", prop.id, "HARGA_LAHAN",
                         std::to_string(prop.buyPrice),
                         "Street buy price must be positive",
                         "Street properties must have a buy price > 0");
            }
            
            if (prop.houseUpgrade < 0) {
                addError(errors, "property.txt", prop.id, "UPG_RUMAH",
                         std::to_string(prop.houseUpgrade),
                         "House upgrade price cannot be negative",
                         "Provide a non-negative value");
            }
            
            if (prop.hotelUpgrade < 0) {
                addError(errors, "property.txt", prop.id, "UPG_HT",
                         std::to_string(prop.hotelUpgrade),
                     "Hotel upgrade price cannot be negative",
                         "Provide a non-negative value");
            }
        }
        
        // RAILROAD should have buyPrice = 0 (auto-acquired)
        if (prop.type == "RAILROAD" && prop.buyPrice != 0) {
            addError(errors, "property.txt", prop.id, "HARGA_LAHAN",
                     std::to_string(prop.buyPrice),
                     "Railroad buy price should be 0",
                     "Railroads are acquired automatically, set HARGA_LAHAN to 0");
        }
        
        // UTILITY should have buyPrice = 0 (auto-acquired)
        if (prop.type == "UTILITY" && prop.buyPrice != 0) {
            addError(errors, "property.txt", prop.id, "HARGA_LAHAN",
                     std::to_string(prop.buyPrice),
                     "Utility buy price should be 0",
                     "Utilities are acquired automatically, set HARGA_LAHAN to 0");
        }
    }
}

void ConfigValidator::validateRentTable(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    for (const auto& prop : properties) {
        if (prop.type != "STREET") {
            continue;  // Only STREET has rent table
        }
        
        // STREET must have exactly 6 rent values (L0-L5)
        if (prop.rentTable.size() != 6) {
            addError(errors, "property.txt", prop.id, "RENT_TABLE",
                     std::to_string(prop.rentTable.size()) + " values",
                     "Street rent table must have exactly 6 values (L0-L5)",
                     "Format: RENT_L0 RENT_L1 RENT_L2 RENT_L3 RENT_L4 RENT_L5");
            continue;
        }
        
        // Rent must be non-decreasing: L0 <= L1 <= L2 <= L3 <= L4 <= L5
        for (size_t i = 1; i < prop.rentTable.size(); ++i) {
            if (prop.rentTable[i] < prop.rentTable[i-1]) {
                addError(errors, "property.txt", prop.id, 
                         "RENT_L" + std::to_string(i),
                         std::to_string(prop.rentTable[i]),
                         "Rent table must be non-decreasing",
                         "RENT_L" + std::to_string(i) + " (" + 
                         std::to_string(prop.rentTable[i]) + 
                         ") should be >= RENT_L" + std::to_string(i-1) + " (" +
                         std::to_string(prop.rentTable[i-1]) + ")");
            }
        }
        
        // All rent values must be non-negative
        for (size_t i = 0; i < prop.rentTable.size(); ++i) {
            if (prop.rentTable[i] < 0) {
                addError(errors, "property.txt", prop.id,
                         "RENT_L" + std::to_string(i),
                         std::to_string(prop.rentTable[i]),
                         "Rent cannot be negative",
                         "Provide a non-negative value");
            }
        }
    }
}

void ConfigValidator::validateColorGroups(
    const std::vector<PropertyData>& properties,
    std::vector<ConfigValidationError>& errors) {
    
    // Count properties per color
    std::map<std::string, std::vector<int>> colorGroups;
    
    for (const auto& prop : properties) {
        if (prop.type == "STREET") {
            colorGroups[prop.color].push_back(prop.id);
        }
    }
    
    // Check each color group
    for (const auto& [color, ids] : colorGroups) {
        // COKLAT and BIRU_TUA should have exactly 2 streets
        if ((color == "COKLAT" || color == "BIRU_TUA") && ids.size() != 2) {
            addError(errors, "property.txt", 0, "COLOR_GROUP", color,
                     "Invalid color group size",
                     "Color group " + color + " should have exactly 2 streets, "
                     "got " + std::to_string(ids.size()));
        }
        
        // Other colors should have exactly 3 streets
        if (color != "COKLAT" && color != "BIRU_TUA" && ids.size() != 3) {
            addError(errors, "property.txt", 0, "COLOR_GROUP", color,
                     "Invalid color group size",
                     "Color group " + color + " should have exactly 3 streets, "
                     "got " + std::to_string(ids.size()));
        }
    }
}

// ============================================================================
// Action Tile Validation
// ============================================================================

void ConfigValidator::validateActionTiles(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& actionTiles = config.actionTiles;
    
    // Check total count
    if (actionTiles.size() != static_cast<size_t>(EXPECTED_ACTION_TILE_COUNT)) {
        addError(errors, "aksi.txt", 0, "", 
                 std::to_string(actionTiles.size()),
                 "Invalid action tile count",
                 "Expected " + std::to_string(EXPECTED_ACTION_TILE_COUNT) + 
                 " action tiles, got " + std::to_string(actionTiles.size()));
    }
    
    validateActionTileIds(actionTiles, errors);
    validateActionTileTypes(actionTiles, errors);
    validateNoOverlapWithProperties(config.properties, actionTiles, errors);
}

void ConfigValidator::validateActionTileIds(
    const std::vector<ActionTileData>& actionTiles,
    std::vector<ConfigValidationError>& errors) {
    
    std::set<int> usedIds;
    
    for (const auto& tile : actionTiles) {
        // Check range
        if (tile.id < MIN_BOARD_POSITION || tile.id > MAX_BOARD_POSITION) {
            addError(errors, "aksi.txt", tile.id, "ID",
                     std::to_string(tile.id),
                     "Action tile ID out of range",
                     "ID must be between " + std::to_string(MIN_BOARD_POSITION) + 
                     " and " + std::to_string(MAX_BOARD_POSITION));
        }
        
        // Check uniqueness
        if (!usedIds.insert(tile.id).second) {
            addError(errors, "aksi.txt", tile.id, "ID",
                     std::to_string(tile.id),
                     "Duplicate action tile ID",
                     "Each action tile must have a unique ID");
        }
        
        // Check code not empty
        if (tile.code.empty()) {
            addError(errors, "aksi.txt", tile.id, "CODE", "",
                     "Action tile code cannot be empty",
                     "Provide a code (e.g., GO, DNU, FES)");
        }
    }
}

void ConfigValidator::validateActionTileTypes(
    const std::vector<ActionTileData>& actionTiles,
    std::vector<ConfigValidationError>& errors) {
    
    for (const auto& tile : actionTiles) {
        if (VALID_ACTION_TYPES.find(tile.type) == VALID_ACTION_TYPES.end()) {
            addError(errors, "aksi.txt", tile.id, "JENIS_PETAK", tile.type,
                     "Invalid action tile type",
                     "Valid types are: SPESIAL, KARTU, PAJAK, FESTIVAL");
        }
    }
}

void ConfigValidator::validateNoOverlapWithProperties(
    const std::vector<PropertyData>& properties,
    const std::vector<ActionTileData>& actionTiles,
    std::vector<ConfigValidationError>& errors) {
    
    std::set<int> propertyIds;
    for (const auto& prop : properties) {
        propertyIds.insert(prop.id);
    }
    
    for (const auto& tile : actionTiles) {
        if (propertyIds.find(tile.id) != propertyIds.end()) {
            addError(errors, "aksi.txt", tile.id, "ID",
                     std::to_string(tile.id),
                     "Action tile ID conflicts with property",
                     "Position " + std::to_string(tile.id) + 
                     " is already occupied by a property");
        }
    }
}

// ============================================================================
// Railroad Validation
// ============================================================================

void ConfigValidator::validateRailroad(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& railroad = config.railroad;
    
    // Must have entries for 1-4 railroads
    for (int i = 1; i <= EXPECTED_RAILROAD_COUNT; ++i) {
        if (railroad.rentTable.find(i) == railroad.rentTable.end()) {
            addError(errors, "railroad.txt", 0, "", std::to_string(i),
                     "Missing railroad rent entry",
                     "Must define rent for " + std::to_string(i) + " railroad(s)");
        }
    }
    
    // Rent values must be positive and non-decreasing
    int prevRent = 0;
    for (int i = 1; i <= EXPECTED_RAILROAD_COUNT; ++i) {
        auto it = railroad.rentTable.find(i);
        if (it != railroad.rentTable.end()) {
            if (it->second <= 0) {
                addError(errors, "railroad.txt", 0, "BIAYA_SEWA",
                         std::to_string(it->second),
                         "Railroad rent must be positive",
                         "Provide a positive value");
            }
            if (it->second < prevRent) {
                addError(errors, "railroad.txt", 0, "BIAYA_SEWA",
                         std::to_string(it->second),
                         "Railroad rent must be non-decreasing",
                         "Rent for " + std::to_string(i) + 
                         " railroads should be >= rent for " +
                         std::to_string(i-1) + " railroads");
            }
            prevRent = it->second;
        }
    }
}

// ============================================================================
// Utility Validation
// ============================================================================

void ConfigValidator::validateUtility(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& utility = config.utility;
    
    // Must have entries for 1-2 utilities
    for (int i = 1; i <= EXPECTED_UTILITY_COUNT; ++i) {
        if (utility.multiplierTable.find(i) == utility.multiplierTable.end()) {
            addError(errors, "utility.txt", 0, "", std::to_string(i),
                     "Missing utility multiplier entry",
                     "Must define multiplier for " + std::to_string(i) + 
                     " utility/ies");
        }
    }
    
    // Multiplier values must be positive
    for (int i = 1; i <= EXPECTED_UTILITY_COUNT; ++i) {
        auto it = utility.multiplierTable.find(i);
        if (it != utility.multiplierTable.end()) {
            if (it->second <= 0) {
                addError(errors, "utility.txt", 0, "FAKTOR_PENGALI",
                         std::to_string(it->second),
                         "Utility multiplier must be positive",
                         "Provide a positive value");
            }
        }
    }
}

// ============================================================================
// Tax Validation
// ============================================================================

void ConfigValidator::validateTax(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& tax = config.tax;
    
    // PPH Flat must be non-negative
    if (tax.pphFlat < 0) {
        addError(errors, "tax.txt", 0, "PPH_FLAT",
                 std::to_string(tax.pphFlat),
                 "PPH flat tax cannot be negative",
                 "Provide a non-negative value");
    }
    
    // PPH Percent must be 0-100
    if (tax.pphPercent < 0 || tax.pphPercent > 100) {
        addError(errors, "tax.txt", 0, "PPH_PERSENTASE",
                 std::to_string(tax.pphPercent),
                 "PPH percentage out of range",
                 "Percentage must be between 0 and 100");
    }
    
    // PBM Flat must be non-negative
    if (tax.pbmFlat < 0) {
        addError(errors, "tax.txt", 0, "PBM_FLAT",
                 std::to_string(tax.pbmFlat),
                 "PBM flat tax cannot be negative",
                 "Provide a non-negative value");
    }
}

// ============================================================================
// Special Tiles Validation
// ============================================================================

void ConfigValidator::validateSpecial(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& special = config.special;
    
    // GO Salary must be positive
    if (special.goSalary <= 0) {
        addError(errors, "special.txt", 0, "GO_SALARY",
                 std::to_string(special.goSalary),
                 "GO salary must be positive",
                 "Provide a positive value");
    }
    
    // Jail Fine must be positive
    if (special.jailFine <= 0) {
        addError(errors, "special.txt", 0, "JAIL_FINE",
                 std::to_string(special.jailFine),
                 "Jail fine must be positive",
                 "Provide a positive value");
    }
}

// ============================================================================
// Misc Validation
// ============================================================================

void ConfigValidator::validateMisc(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    const auto& misc = config.misc;
    
    // Max turn must be positive (or 0 for bankruptcy mode)
    if (misc.maxTurn < 0) {
        addError(errors, "misc.txt", 0, "MAX_TURN",
                 std::to_string(misc.maxTurn),
                 "Max turns cannot be negative",
                 "Use 0 or positive value (0 = bankruptcy mode)");
    }
    
    // Starting balance must be positive
    if (misc.startingBalance <= 0) {
        addError(errors, "misc.txt", 0, "SALDO_AWAL",
                 std::to_string(misc.startingBalance),
                 "Starting balance must be positive",
                 "Provide a positive value");
    }
}

// ============================================================================
// Cross-File Validation
// ============================================================================

void ConfigValidator::validateCrossReferences(
    const Config& config,
    std::vector<ConfigValidationError>& errors) {
    
    // Count railroad properties
    int railroadCount = 0;
    int utilityCount = 0;
    
    for (const auto& prop : config.properties) {
        if (prop.type == "RAILROAD") {
            ++railroadCount;
        } else if (prop.type == "UTILITY") {
            ++utilityCount;
        }
    }
    
    // Validate railroad count matches rent table
    if (railroadCount != EXPECTED_RAILROAD_COUNT) {
        addError(errors, "property.txt", 0, "",
                 std::to_string(railroadCount),
                 "Railroad count mismatch",
                 "Expected " + std::to_string(EXPECTED_RAILROAD_COUNT) + 
                 " railroads in properties, got " + 
                 std::to_string(railroadCount));
    }
    
    // Validate utility count matches multiplier table
    if (utilityCount != EXPECTED_UTILITY_COUNT) {
        addError(errors, "property.txt", 0, "",
                 std::to_string(utilityCount),
                 "Utility count mismatch",
                 "Expected " + std::to_string(EXPECTED_UTILITY_COUNT) + 
                 " utilities in properties, got " + 
                 std::to_string(utilityCount));
    }
    
    // Check that property IDs + action tile IDs = 40 (complete board)
    std::set<int> occupiedPositions;
    for (const auto& prop : config.properties) {
        occupiedPositions.insert(prop.id);
    }
    for (const auto& tile : config.actionTiles) {
        occupiedPositions.insert(tile.id);
    }
    
    if (occupiedPositions.size() != MAX_BOARD_POSITION) {
        addError(errors, "config", 0, "",
                 std::to_string(occupiedPositions.size()),
                 "Incomplete board configuration",
                 "Expected " + std::to_string(MAX_BOARD_POSITION) + 
                 " positions (properties + action tiles), got " +
                 std::to_string(occupiedPositions.size()) + 
                 ". Check for missing or duplicate positions.");
        
        // Report which positions are missing
        std::string missing;
        for (int i = MIN_BOARD_POSITION; i <= MAX_BOARD_POSITION; ++i) {
            if (occupiedPositions.find(i) == occupiedPositions.end()) {
                if (!missing.empty()) missing += ", ";
                missing += std::to_string(i);
            }
        }
        if (!missing.empty()) {
            addError(errors, "config", 0, "",
                     missing,
                     "Missing board positions",
                     "Add properties or action tiles at these positions");
        }
    }
}

// ============================================================================
// Helper Methods
// ============================================================================

void ConfigValidator::addError(
    std::vector<ConfigValidationError>& errors,
    const std::string& file,
    int lineNumber,
    const std::string& field,
    const std::string& value,
    const std::string& message,
    const std::string& suggestion) {
    
    ConfigValidationError error;
    error.file = file;
    error.lineNumber = lineNumber;
    error.field = field;
    error.value = value;
    error.message = message;
    error.suggestion = suggestion;
    errors.push_back(std::move(error));
}
