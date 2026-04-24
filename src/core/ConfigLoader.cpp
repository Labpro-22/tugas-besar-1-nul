#include "core/ConfigLoader.hpp"
#include "config/ActionTileData.hpp"
#include "exception/ConfigException.hpp"
#include "utils/ParseUtils.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

Config ConfigLoader::loadAll(const std::string& path) const {
    Config config;
    config.properties = loadProperty(path + "/property.txt");
    config.actionTiles = loadActionTiles(path + "/aksi.txt");
    config.railroad = loadRailroad(path + "/railroad.txt");
    config.utility = loadUtility(path + "/utility.txt");
    config.tax = loadTax(path + "/tax.txt");
    config.special = loadSpecial(path + "/special.txt");
    config.misc = loadMisc(path + "/misc.txt");
    return config;
}

std::vector<PropertyData>
ConfigLoader::loadProperty(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open property config file: " + file);
    }

    std::vector<PropertyData> result;
    std::string line;
    int lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;

        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty() || tokens[0][0] == '#') {
            continue;
        }

        if (tokens.size() < 7) {
            throw ConfigException("Invalid property row at line " +
                                  std::to_string(lineNumber) +
                                  ": expected at least 7 columns");
        }

        PropertyData property;

        if (!tryParseInt(tokens[0], property.id)) {
            if (tokens[0] == "ID") {
                continue;
            }
            throw ConfigException("Invalid property ID at line " +
                                  std::to_string(lineNumber));
        }

        property.code = tokens[1];
        property.name = tokens[2];
        property.type = tokens[3];
        property.color = tokens[4];

        std::vector<int> numericTail;
        numericTail.reserve(tokens.size() - 5);
        for (std::size_t i = 5; i < tokens.size(); ++i) {
            int value = 0;
            if (tryParseInt(tokens[i], value)) {
                numericTail.push_back(value);
            }
        }

        if (property.type == "STREET") {
            if (numericTail.size() < 10) {
                throw ConfigException(
                    "Invalid STREET row at line " + std::to_string(lineNumber) +
                    ": expected at least 10 numeric values after color");
            }

            property.buyPrice = numericTail[0];
            property.mortgageValue = numericTail[1];
            property.houseUpgrade = numericTail[2];
            property.hotelUpgrade = numericTail[3];
            property.rentTable.assign(numericTail.begin() + 4,
                                      numericTail.begin() + 10);
        } else {
            if (numericTail.size() < 2) {
                throw ConfigException(
                    "Invalid non-STREET row at line " +
                    std::to_string(lineNumber) +
                    ": expected at least buy price and mortgage value");
            }

            property.buyPrice = numericTail[0];
            property.mortgageValue = numericTail[1];
            property.houseUpgrade = 0;
            property.hotelUpgrade = 0;

            if (numericTail.size() > 2) {
                property.rentTable.assign(numericTail.begin() + 2,
                                          numericTail.end());
            }
        }

        result.push_back(property);
    }

    return result;
}

RailroadConfig ConfigLoader::loadRailroad(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open property railroad config: " +
                              file);
    }

    RailroadConfig config;
    std::string line;
    int lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        const std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty()) {
            continue;
        }

        int count = 0;
        int rent = 0;

        const bool firstIsInt = tryParseInt(tokens[0], count);
        const bool secondIsInt =
            (tokens.size() > 1) && tryParseInt(tokens[1], rent);

        if (!firstIsInt) {
            continue; // header row
        }
        if (!secondIsInt) {
            throw ConfigException("Invalid railroad row at line " +
                                  std::to_string(lineNumber));
        }

        config.rentTable[count] = rent;
    }
    return config;
}

UtilityConfig ConfigLoader::loadUtility(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open utility config file: " + file);
    }

    UtilityConfig config;
    std::string line;
    int lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        const std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty()) {
            continue;
        }

        int count = 0;
        int factor = 0;
        const bool firstIsInt = tryParseInt(tokens[0], count);
        const bool secondIsInt =
            (tokens.size() > 1) && tryParseInt(tokens[1], factor);

        if (!firstIsInt) {
            continue; // header row
        }
        if (!secondIsInt) {
            throw ConfigException("Invalid utility row at line " +
                                  std::to_string(lineNumber));
        }

        config.multiplierTable[count] = factor;
    }

    return config;
}

TaxConfig ConfigLoader::loadTax(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open tax config file: " + file);
    }

    int lineNumber = 0;
    while (true) {
        std::vector<std::string> tokens = readNextDataTokens(in, lineNumber);
        if (tokens.empty()) {
            throw ConfigException("Tax config has no data row: " + file);
        }

        int pphFlat = 0;
        int pphPercent = 0;
        int pbmFlat = 0;
        if (!tryParseInt(tokens[0], pphFlat)) {
            continue; // header row
        }
        if (tokens.size() < 3 || !tryParseInt(tokens[1], pphPercent) ||
            !tryParseInt(tokens[2], pbmFlat)) {
            throw ConfigException("Invalid tax row at line " +
                                  std::to_string(lineNumber));
        }

        TaxConfig config;
        config.pphFlat = pphFlat;
        config.pphPercent = pphPercent;
        config.pbmFlat = pbmFlat;
        return config;
    }
}

SpecialConfig ConfigLoader::loadSpecial(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open special config file: " + file);
    }

    int lineNumber = 0;
    while (true) {
        std::vector<std::string> tokens = readNextDataTokens(in, lineNumber);
        if (tokens.empty()) {
            throw ConfigException("Special config has no data row: " + file);
        }

        int goSalary = 0;
        int jailFine = 0;
        if (!tryParseInt(tokens[0], goSalary)) {
            continue; // header row
        }
        if (tokens.size() < 2 || !tryParseInt(tokens[1], jailFine)) {
            throw ConfigException("Invalid special row at line " +
                                  std::to_string(lineNumber));
        }

        SpecialConfig config;
        config.goSalary = goSalary;
        config.jailFine = jailFine;
        return config;
    }
}

MiscConfig ConfigLoader::loadMisc(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open misc config file: " + file);
    }

    int lineNumber = 0;
    while (true) {
        std::vector<std::string> tokens = readNextDataTokens(in, lineNumber);
        if (tokens.empty()) {
            throw ConfigException("Misc config has no data row: " + file);
        }

        int maxTurn = 0;
        int startingBalance = 0;
        if (!tryParseInt(tokens[0], maxTurn)) {
            continue; // header row
        }
        if (tokens.size() < 2 || !tryParseInt(tokens[1], startingBalance)) {
            throw ConfigException("Invalid misc row at line " +
                                  std::to_string(lineNumber));
        }

        MiscConfig config;
        config.maxTurn = maxTurn;
        config.startingBalance = startingBalance;
        return config;
    }
}

std::vector<ActionTileData>
ConfigLoader::loadActionTiles(const std::string& file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw ConfigException("Failed to open action tiles config file: " +
                              file);
    }

    std::vector<ActionTileData> result;
    std::string line;
    int lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;

        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty() || tokens[0][0] == '#') {
            continue;
        }

        // Format: ID KODE NAMA JENIS_PETAK WARNA
        if (tokens.size() < 5) {
            throw ConfigException("Invalid action tile row at line " +
                                  std::to_string(lineNumber) +
                                  ": expected at least 5 columns (ID KODE NAMA "
                                  "JENIS_PETAK WARNA)");
        }

        ActionTileData tile;

        // Parse ID
        if (!tryParseInt(tokens[0], tile.id)) {
            if (tokens[0] == "ID") {
                continue; // Skip header row
            }
            throw ConfigException("Invalid action tile ID at line " +
                                  std::to_string(lineNumber) + ": '" +
                                  tokens[0] + "'");
        }

        tile.code = tokens[1];
        tile.name = tokens[2];
        tile.type = tokens[3];
        tile.color = tokens[4];

        result.push_back(tile);
    }

    return result;
}
