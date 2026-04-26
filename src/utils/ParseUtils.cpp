#include "utils/ParseUtils.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

bool tryParseInt(const std::string& token, int& out) {
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

std::vector<std::string> splitWhitespace(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

bool isCommentToken(const std::string& token) {
    return !token.empty() && token[0] == '#';
}

std::vector<std::string> readNextDataTokens(std::ifstream& in,
                                            int& lineNumber) {
    std::string line;
    while (std::getline(in, line)) {
        ++lineNumber;
        std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty()) {
            continue;
        }
        if (isCommentToken(tokens[0])) {
            continue;
        }
        return tokens;
    }
    return {};
}

std::vector<int> parseNumericTail(const std::vector<std::string>& tokens,
                                  std::size_t startIndex) {
    std::vector<int> values;
    values.reserve(tokens.size() - startIndex);

    for (std::size_t i = startIndex; i < tokens.size(); ++i) {
        int value = 0;
        if (tryParseInt(tokens[i], value)) {
            values.push_back(value);
        }
    }

    return values;
}

std::vector<std::string> splitBy(const std::string& s, char delimiter) {
    std::vector<std::string> parts;
    std::string part;
    std::istringstream ss(s);
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

bool parseBoolInt(const std::string& token) {
    int value = 0;
    if (!tryParseInt(token, value)) {
        throw std::runtime_error("Invalid integer boolean token: " + token);
    }
    return value != 0;
}
