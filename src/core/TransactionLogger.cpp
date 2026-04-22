#include "core/TransactionLogger.hpp"
#include "utils/SerializeUtils.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

void TransactionLogger::log(int turn,
                            const std::string& user,
                            const std::string& action,
                            const std::string& detail) {
    entries.push_back(LogEntry{turn, user, action, detail});
}

std::vector<LogEntry> TransactionLogger::getAll() const {
    return entries;
}

std::vector<LogEntry> TransactionLogger::getLast(int n) const {
    if (n <= 0 || entries.empty()) {
        return {};
    }

    const int start = (n >= static_cast<int>(entries.size()))
                          ? 0
                          : static_cast<int>(entries.size()) - n;

    return std::vector<LogEntry>(entries.begin() + start, entries.end());
}

void TransactionLogger::printLogs(int n) const {
    std::vector<LogEntry> logsToPrint;
    if (n > 0) {
        logsToPrint = getLast(n);
    } else {
        logsToPrint = getAll();
    }

    std::cout << "=== Log Transaksi Penuh ===\n\n";
    for (const LogEntry& entry : logsToPrint) {
        std::cout << entry.toString() << '\n';
    }
}

std::string TransactionLogger::serialize() const {
    std::ostringstream oss;

    for (const LogEntry& entry : entries) {
        oss << entry.turn << '\t' << escapeField(entry.username) << '\t'
            << escapeField(entry.actionType) << '\t'
            << escapeField(entry.detail) << '\n';
    }

    return oss.str();
}

void TransactionLogger::deserialize(const std::string& data) {
    entries.clear();

    std::istringstream input(data);
    std::string line;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::string turnRaw;
        std::string usernameRaw;
        std::string actionRaw;
        std::string detailRaw;
        if (!splitSerializedLine(
                line, turnRaw, usernameRaw, actionRaw, detailRaw)) {
            throw std::runtime_error("Invalid serialized log entry format");
        }

        LogEntry entry;
        entry.turn = std::stoi(turnRaw);
        entry.username = unescapeField(usernameRaw);
        entry.actionType = unescapeField(actionRaw);
        entry.detail = unescapeField(detailRaw);

        entries.push_back(entry);
    }
}
