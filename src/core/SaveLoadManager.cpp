#include "core/SaveLoadManager.hpp"

#include "exception/SaveLoadException.hpp"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "states/StateParser.hpp"
#include "states/StateSerializer.hpp"
#include "utils/ParseUtils.hpp"

namespace {

std::string readRequiredNonEmptyLine(std::istream& in,
                                     const std::string& errorMessage) {
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            return line;
        }
    }
    throw SaveLoadException(errorMessage);
}

std::vector<std::string>
readLines(std::istream& in, int count, const std::string& sectionName) {
    if (count < 0) {
        throw SaveLoadException("Invalid negative count in section: " +
                                sectionName);
    }

    std::vector<std::string> lines;
    lines.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        std::string line;
        if (!std::getline(in, line)) {
            throw SaveLoadException("Unexpected EOF while reading section: " +
                                    sectionName);
        }
        lines.push_back(line);
    }
    return lines;
}

} // namespace

void SaveLoadManager::save(const GameState& state,
                           const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw SaveLoadException("Failed to open save file for writing: " +
                                filename);
    }

    out << state.currentTurn << ' ' << state.maxTurn << '\n';
    out << serializePlayers(state.players);

    for (std::size_t i = 0; i < state.turnOrder.size(); ++i) {
        const int idx = state.turnOrder[i];
        if (idx < 0 || idx >= static_cast<int>(state.players.size())) {
            throw SaveLoadException(
                "TURN_ORDER contains invalid player index while saving");
        }
        if (i > 0) {
            out << ' ';
        }
        out << state.players[idx].username;
    }
    out << '\n';

    if (state.activePlayerIdx < 0 ||
        state.activePlayerIdx >= static_cast<int>(state.players.size())) {
        throw SaveLoadException(
            "ACTIVE_PLAYER_IDX is invalid while saving game state");
    }
    out << state.players[state.activePlayerIdx].username << '\n';

    out << serializeProperties(state.properties);
    out << serializeDeck(state.deckState);
    out << serializeLog(state.log);
}

GameState SaveLoadManager::load(const std::string& filename) const {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw SaveLoadException("Failed to open save file for reading: " +
                                filename);
    }

    GameState state;
    const std::string turnLine = readRequiredNonEmptyLine(
        in, "Missing first line <TURN_SAAT_INI> <MAX_TURN>");
    {
        const std::vector<std::string> parts = splitWhitespace(turnLine);
        if (parts.size() < 2 || !tryParseInt(parts[0], state.currentTurn) ||
            !tryParseInt(parts[1], state.maxTurn)) {
            throw SaveLoadException("Invalid first line format, expected: "
                                    "<TURN_SAAT_INI> <MAX_TURN>");
        }
    }

    const std::string playerCountLine =
        readRequiredNonEmptyLine(in, "Missing <JUMLAH_PEMAIN>");
    int playerCount = 0;
    const std::vector<std::string> playerCountTokens =
        splitWhitespace(playerCountLine);
    if (playerCountTokens.empty() ||
        !tryParseInt(playerCountTokens[0], playerCount) || playerCount < 0) {
        throw SaveLoadException("Invalid <JUMLAH_PEMAIN>");
    }

    std::vector<std::string> playerBlock;
    playerBlock.push_back(std::to_string(playerCount));
    for (int i = 0; i < playerCount; ++i) {
        const std::string playerStateLine = readRequiredNonEmptyLine(
            in, "Missing <STATE_PEMAIN_i> in player block");
        playerBlock.push_back(playerStateLine);

        const std::string handCountLine = readRequiredNonEmptyLine(
            in, "Missing <JUMLAH_KARTU_TANGAN> in player block");
        playerBlock.push_back(handCountLine);

        int handCount = 0;
        const std::vector<std::string> handCountTokens =
            splitWhitespace(handCountLine);
        if (handCountTokens.empty() ||
            !tryParseInt(handCountTokens[0], handCount) || handCount < 0) {
            throw SaveLoadException("Invalid <JUMLAH_KARTU_TANGAN>");
        }

        for (int h = 0; h < handCount; ++h) {
            playerBlock.push_back(readRequiredNonEmptyLine(
                in, "Missing <JENIS_KARTU_i> line in hand block"));
        }
    }
    state.players = parsePlayers(playerBlock);

    const std::string turnOrderLine =
        readRequiredNonEmptyLine(in, "Missing turn order line");
    const std::vector<std::string> turnOrderUsernames =
        splitWhitespace(turnOrderLine);
    if (static_cast<int>(turnOrderUsernames.size()) != playerCount) {
        throw SaveLoadException("Turn order count must match <JUMLAH_PEMAIN>");
    }

    std::map<std::string, int> usernameToIndex;
    for (std::size_t i = 0; i < state.players.size(); ++i) {
        usernameToIndex[state.players[i].username] = static_cast<int>(i);
    }

    state.turnOrder.clear();
    for (const std::string& username : turnOrderUsernames) {
        auto it = usernameToIndex.find(username);
        if (it == usernameToIndex.end()) {
            throw SaveLoadException("Unknown username in turn order: " +
                                    username);
        }
        state.turnOrder.push_back(it->second);
    }

    const std::string activePlayerLine =
        readRequiredNonEmptyLine(in, "Missing <GILIRAN_AKTIF_SAAT_INI>");
    {
        const std::vector<std::string> activeTokens =
            splitWhitespace(activePlayerLine);
        if (activeTokens.empty()) {
            throw SaveLoadException("Invalid active player line");
        }

        auto it = usernameToIndex.find(activeTokens[0]);
        if (it == usernameToIndex.end()) {
            throw SaveLoadException("Unknown active player username: " +
                                    activeTokens[0]);
        }
        state.activePlayerIdx = it->second;
    }

    const std::string propertyCountLine =
        readRequiredNonEmptyLine(in, "Missing <JUMLAH_PROPERTI>");
    int propertyCount = 0;
    const std::vector<std::string> propertyCountTokens =
        splitWhitespace(propertyCountLine);
    if (propertyCountTokens.empty() ||
        !tryParseInt(propertyCountTokens[0], propertyCount) ||
        propertyCount < 0) {
        throw SaveLoadException("Invalid <JUMLAH_PROPERTI>");
    }
    std::vector<std::string> propertyBlock;
    propertyBlock.push_back(std::to_string(propertyCount));
    std::vector<std::string> propertyLines =
        readLines(in, propertyCount, "STATE_PROPERTI");
    propertyBlock.insert(
        propertyBlock.end(), propertyLines.begin(), propertyLines.end());
    state.properties = parseProperties(propertyBlock);

    const std::string deckCountLine =
        readRequiredNonEmptyLine(in, "Missing <JUMLAH_KARTU_DECK_KEMAMPUAN>");
    int deckCount = 0;
    const std::vector<std::string> deckCountTokens =
        splitWhitespace(deckCountLine);
    if (deckCountTokens.empty() ||
        !tryParseInt(deckCountTokens[0], deckCount) || deckCount < 0) {
        throw SaveLoadException("Invalid <JUMLAH_KARTU_DECK_KEMAMPUAN>");
    }
    std::vector<std::string> deckBlock;
    deckBlock.push_back(std::to_string(deckCount));
    std::vector<std::string> deckLines = readLines(in, deckCount, "STATE_DECK");
    deckBlock.insert(deckBlock.end(), deckLines.begin(), deckLines.end());
    state.deckState = parseDeck(deckBlock);

    const std::string logCountLine =
        readRequiredNonEmptyLine(in, "Missing <JUMLAH_ENTRI_LOG>");
    int logCount = 0;
    const std::vector<std::string> logCountTokens =
        splitWhitespace(logCountLine);
    if (logCountTokens.empty() || !tryParseInt(logCountTokens[0], logCount) ||
        logCount < 0) {
        throw SaveLoadException("Invalid <JUMLAH_ENTRI_LOG>");
    }
    std::vector<std::string> logBlock;
    logBlock.push_back(std::to_string(logCount));
    std::vector<std::string> logLines = readLines(in, logCount, "STATE_LOG");
    logBlock.insert(logBlock.end(), logLines.begin(), logLines.end());
    state.log = parseLog(logBlock);

    return state;
}

std::string SaveLoadManager::serializePlayers(
    const std::vector<PlayerState>& players) const {
    StateSerializer serializer;
    return serializer.serializePlayers(players);
}

std::string SaveLoadManager::serializeProperties(
    const std::vector<PropertyState>& properties) const {
    StateSerializer serializer;
    return serializer.serializeProperties(properties);
}

std::string
SaveLoadManager::serializeDeck(const std::vector<std::string>& deck) const {
    StateSerializer serializer;
    return serializer.serializeDeck(deck);
}

std::string
SaveLoadManager::serializeLog(const std::vector<LogEntry>& logs) const {
    StateSerializer serializer;
    return serializer.serializeLog(logs);
}

std::vector<PlayerState>
SaveLoadManager::parsePlayers(const std::vector<std::string>& lines) const {
    StateParser parser;
    return parser.parsePlayers(lines);
}

std::vector<PropertyState>
SaveLoadManager::parseProperties(const std::vector<std::string>& lines) const {
    StateParser parser;
    return parser.parseProperties(lines);
}

std::vector<std::string>
SaveLoadManager::parseDeck(const std::vector<std::string>& lines) const {
    StateParser parser;
    return parser.parseDeck(lines);
}

std::vector<LogEntry>
SaveLoadManager::parseLog(const std::vector<std::string>& lines) const {
    StateParser parser;
    return parser.parseLog(lines);
}
