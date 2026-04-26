#pragma once

#include <set>
#include <string>
#include <vector>

#include "config/Config.hpp"

/**
 * Validation error structure for detailed error reporting
 */
struct ConfigValidationError {
    std::string file;       // Source file (e.g., "property.txt")
    int lineNumber;         // Line number in file
    std::string field;      // Field name (e.g., "ID", "type")
    std::string value;      // Invalid value
    std::string message;    // Error message
    std::string suggestion; // How to fix

    std::string toString() const;
};

/**
 * Exception thrown when config validation fails
 */
class ConfigValidationException : public std::exception {
  private:
    std::vector<ConfigValidationError> errors_;
    mutable std::string message_;

  public:
    explicit ConfigValidationException(
        std::vector<ConfigValidationError> errors);

    const char* what() const noexcept override;

    const std::vector<ConfigValidationError>& getErrors() const {
        return errors_;
    }

    bool hasErrors() const { return !errors_.empty(); }
};

/**
 * ConfigValidator - Comprehensive validation for game configuration
 *
 * Validation levels:
 * 1. Syntax: Format, types, required fields
 * 2. Semantic: Ranges, uniqueness, valid enums
 * 3. Business rules: Game logic consistency
 * 4. Cross-file: Consistency between config files
 */
class ConfigValidator {
  public:
    // Validation constants
    static constexpr int MIN_PROPERTY_ID = 1;
    static constexpr int MAX_PROPERTY_ID = 40;
    static constexpr int EXPECTED_PROPERTY_COUNT = 28;
    static constexpr int EXPECTED_STREET_COUNT = 22;
    static constexpr int EXPECTED_RAILROAD_COUNT = 4;
    static constexpr int EXPECTED_UTILITY_COUNT = 2;
    static constexpr int EXPECTED_ACTION_TILE_COUNT = 12;
    static constexpr int MIN_BOARD_POSITION = 1;
    static constexpr int MAX_BOARD_POSITION = 40;

    static const std::set<std::string> VALID_PROPERTY_TYPES;
    static const std::set<std::string> VALID_COLORS;
    static const std::set<std::string> VALID_ACTION_TYPES;

    /**
     * Run all validations on the config
     * @throws ConfigValidationException if any validation fails
     */
    static void validate(const Config& config);

    /**
     * Validate properties configuration
     */
    static void validateProperties(const Config& config,
                                   std::vector<ConfigValidationError>& errors);

    /**
     * Validate action tiles configuration
     */
    static void validateActionTiles(const Config& config,
                                    std::vector<ConfigValidationError>& errors);

    /**
     * Validate railroad configuration
     */
    static void validateRailroad(const Config& config,
                                 std::vector<ConfigValidationError>& errors);

    /**
     * Validate utility configuration
     */
    static void validateUtility(const Config& config,
                                std::vector<ConfigValidationError>& errors);

    /**
     * Validate tax configuration
     */
    static void validateTax(const Config& config,
                            std::vector<ConfigValidationError>& errors);

    /**
     * Validate special tiles configuration
     */
    static void validateSpecial(const Config& config,
                                std::vector<ConfigValidationError>& errors);

    /**
     * Validate misc configuration
     */
    static void validateMisc(const Config& config,
                             std::vector<ConfigValidationError>& errors);

    /**
     * Cross-file validation
     */
    static void
    validateCrossReferences(const Config& config,
                            std::vector<ConfigValidationError>& errors);

  private:
    // Helper methods for property validation
    static void validatePropertyIds(const std::vector<PropertyData>& properties,
                                    std::vector<ConfigValidationError>& errors);
    static void
    validatePropertyCodes(const std::vector<PropertyData>& properties,
                          std::vector<ConfigValidationError>& errors);
    static void
    validatePropertyTypes(const std::vector<PropertyData>& properties,
                          std::vector<ConfigValidationError>& errors);
    static void
    validatePropertyColors(const std::vector<PropertyData>& properties,
                           std::vector<ConfigValidationError>& errors);
    static void
    validatePropertyPrices(const std::vector<PropertyData>& properties,
                           std::vector<ConfigValidationError>& errors);
    static void validateRentTable(const std::vector<PropertyData>& properties,
                                  std::vector<ConfigValidationError>& errors);
    static void validateColorGroups(const std::vector<PropertyData>& properties,
                                    std::vector<ConfigValidationError>& errors);
    static void
    validatePropertyCounts(const std::vector<PropertyData>& properties,
                           std::vector<ConfigValidationError>& errors);

    // Helper methods for action tile validation
    static void
    validateActionTileIds(const std::vector<ActionTileData>& actionTiles,
                          std::vector<ConfigValidationError>& errors);
    static void
    validateActionTileTypes(const std::vector<ActionTileData>& actionTiles,
                            std::vector<ConfigValidationError>& errors);
    static void validateNoOverlapWithProperties(
        const std::vector<PropertyData>& properties,
        const std::vector<ActionTileData>& actionTiles,
        std::vector<ConfigValidationError>& errors);

    // Helper to add an error
    static void addError(std::vector<ConfigValidationError>& errors,
                         const std::string& file,
                         int lineNumber,
                         const std::string& field,
                         const std::string& value,
                         const std::string& message,
                         const std::string& suggestion);
};
