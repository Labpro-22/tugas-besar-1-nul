#include "utils/SerializeUtils.hpp"
#include <string>

std::string escapeField(const std::string& value) {
    std::string out;
    out.reserve(value.size());

    for (char c : value) {
        if (c == '\\') {
            out += "\\\\";
        } else if (c == '\t') {
            out += "\\t";
        } else if (c == '\n') {
            out += "\\n";
        } else {
            out.push_back(c);
        }
    }

    return out;
}

std::string unescapeField(const std::string& value) {
    std::string out;
    out.reserve(value.size());

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            const char next = value[i + 1];
            if (next == '\\') {
                out.push_back('\\');
                ++i;
                continue;
            }
            if (next == 't') {
                out.push_back('\t');
                ++i;
                continue;
            }
            if (next == 'n') {
                out.push_back('\n');
                ++i;
                continue;
            }
        }
        out.push_back(value[i]);
    }

    return out;
}

bool splitSerializedLine(const std::string& line,
                         std::string& turn,
                         std::string& username,
                         std::string& action,
                         std::string& detail) {
    std::size_t p1 = line.find('\t');
    if (p1 == std::string::npos) {
        return false;
    }

    std::size_t p2 = line.find('\t', p1 + 1);
    if (p2 == std::string::npos) {
        return false;
    }

    std::size_t p3 = line.find('\t', p2 + 1);
    if (p3 == std::string::npos) {
        return false;
    }

    turn = line.substr(0, p1);
    username = line.substr(p1 + 1, p2 - p1 - 1);
    action = line.substr(p2 + 1, p3 - p2 - 1);
    detail = line.substr(p3 + 1);
    return true;
}
