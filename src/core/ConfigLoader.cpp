#include "core/ConfigLoader.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

bool tryParseInt(const std::string &token, int &out) {
    if (token.empty()) {
        return false;
    }

    std::size_t i = 0;
    if (token[0] == '+' || token[0] == '-') {
        i = 1;
        if (token.size() == 1) {
            return false;
        }
    }

    for (; i < token.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(token[i]))) {
            return false;
        }
    }

    out = std::stoi(token);
    return true;
}

std::vector<std::string> splitWhitespace(const std::string &line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace

Config ConfigLoader::loadAll(const std::string &path) const {
    Config config;
    config.properties = loadProperty(path + "/property.txt");
    config.railroad = loadRailroad(path + "/railroad.txt");
    config.utility = loadUtility(path + "/utility.txt");
    config.tax = loadTax(path + "/tax.txt");
    config.special = loadSpecial(path + "/special.txt");
    config.misc = loadMisc(path + "/misc.txt");
    return config;
}

std::vector<PropertyData>
ConfigLoader::loadProperty(const std::string &file) const {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open property config file: " +
                                 file);
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
            throw std::runtime_error("Invalid property row at line " +
                                     std::to_string(lineNumber) +
                                     ": expected at least 7 columns");
        }

        PropertyData property;

        if (!tryParseInt(tokens[0], property.id)) {
            if (tokens[0] == "ID") {
                continue;
            }
            throw std::runtime_error("Invalid property ID at line " +
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
                throw std::runtime_error(
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
                throw std::runtime_error(
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

RailroadConfig ConfigLoader::loadRailroad(const std::string &file) const {
    (void)file;
    return {};
}

UtilityConfig ConfigLoader::loadUtility(const std::string &file) const {
    (void)file;
    return {};
}

TaxConfig ConfigLoader::loadTax(const std::string &file) const {
    (void)file;
    return {};
}

SpecialConfig ConfigLoader::loadSpecial(const std::string &file) const {
    (void)file;
    return {};
}

MiscConfig ConfigLoader::loadMisc(const std::string &file) const {
    (void)file;
    return {};
}
