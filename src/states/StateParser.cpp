#include "states/StateParser.hpp"

#include <sstream>
#include <stdexcept>

#include "utils/ParseUtils.hpp"

std::vector<CardState>
StateParser::parseHand(const std::string& serializedHand) const {
    std::vector<CardState> hand;
    if (serializedHand.empty()) {
        return hand;
    }

    const std::vector<std::string> lines = splitBy(serializedHand, '\n');
    if (lines.empty()) {
        return hand;
    }

    int cardCount = 0;
    if (!tryParseInt(lines[0], cardCount) || cardCount < 0) {
        throw std::runtime_error("Invalid hand card count");
    }

    if (static_cast<int>(lines.size()) < cardCount + 1) {
        throw std::runtime_error("Insufficient card lines in hand block");
    }

    for (int i = 0; i < cardCount; ++i) {
        const std::vector<std::string> tokens = splitWhitespace(lines[i + 1]);
        if (tokens.empty()) {
            throw std::runtime_error("Invalid empty card line in hand block");
        }

        CardState card;
        card.type = tokens[0];

        if (tokens.size() >= 2) {
            int value = 0;
            if (!tryParseInt(tokens[1], value)) {
                throw std::runtime_error("Invalid card value: " + lines[i + 1]);
            }
            card.value = value;
        }

        if (tokens.size() >= 3) {
            int duration = 0;
            if (!tryParseInt(tokens[2], duration)) {
                throw std::runtime_error("Invalid card duration: " +
                                         lines[i + 1]);
            }
            card.remainingDuration = duration;
        }

        hand.push_back(card);
    }

    return hand;
}

std::vector<PlayerState>
StateParser::parsePlayers(const std::vector<std::string>& lines) const {
    std::vector<PlayerState> players;
    if (lines.empty()) {
        return players;
    }

    int playerCount = 0;
    if (!tryParseInt(lines[0], playerCount) || playerCount < 0) {
        throw std::runtime_error("Invalid player count line");
    }

    std::size_t idx = 1;
    for (int i = 0; i < playerCount; ++i) {
        if (idx >= lines.size()) {
            throw std::runtime_error("Unexpected EOF while parsing players");
        }

        const std::vector<std::string> head = splitWhitespace(lines[idx++]);
        if (head.size() < 4) {
            throw std::runtime_error("Invalid player state line");
        }

        int balance = 0;
        if (!tryParseInt(head[1], balance)) {
            throw std::runtime_error("Invalid player balance value");
        }

        if (idx >= lines.size()) {
            throw std::runtime_error("Missing player hand count line");
        }

        int handCount = 0;
        const std::vector<std::string> handCountTokens =
            splitWhitespace(lines[idx]);
        if (handCountTokens.empty() ||
            !tryParseInt(handCountTokens[0], handCount) || handCount < 0) {
            throw std::runtime_error("Invalid player hand count line");
        }

        std::string handBlock = lines[idx++] + '\n';
        for (int h = 0; h < handCount; ++h) {
            if (idx >= lines.size()) {
                throw std::runtime_error("Unexpected EOF while parsing hand");
            }
            handBlock += lines[idx++] + '\n';
        }

        PlayerState player;
        player.username = head[0];
        player.balance = balance;
        player.positionCode = head[2];
        player.status = head[3];
        player.hand = parseHand(handBlock);

        players.push_back(player);
    }

    return players;
}

std::vector<PropertyState>
StateParser::parseProperties(const std::vector<std::string>& lines) const {
    std::vector<PropertyState> properties;
    if (lines.empty()) {
        return properties;
    }

    int propertyCount = 0;
    if (!tryParseInt(lines[0], propertyCount) || propertyCount < 0) {
        throw std::runtime_error("Invalid property count line");
    }

    if (static_cast<int>(lines.size()) < propertyCount + 1) {
        throw std::runtime_error("Insufficient property lines");
    }

    for (int i = 0; i < propertyCount; ++i) {
        const std::vector<std::string> fields = splitWhitespace(lines[i + 1]);
        if (fields.size() < 7) {
            throw std::runtime_error("Invalid property line: " + lines[i + 1]);
        }

        int festivalMult = 0;
        int festivalDur = 0;
        if (!tryParseInt(fields[4], festivalMult) ||
            !tryParseInt(fields[5], festivalDur)) {
            throw std::runtime_error("Invalid property festival fields");
        }

        PropertyState property;
        property.code = fields[0];
        property.type = fields[1];
        property.owner = fields[2];
        property.status = fields[3];
        property.festivalMult = festivalMult;
        property.festivalDur = festivalDur;

        if (fields[6] == "H") {
            property.isHotel = true;
            property.buildingCount = 0;
        } else {
            int buildingCount = 0;
            if (!tryParseInt(fields[6], buildingCount)) {
                throw std::runtime_error("Invalid building count: " +
                                         lines[i + 1]);
            }
            property.isHotel = false;
            property.buildingCount = buildingCount;
        }

        properties.push_back(property);
    }

    return properties;
}

std::vector<std::string>
StateParser::parseDeck(const std::vector<std::string>& lines) const {
    std::vector<std::string> deck;
    if (lines.empty()) {
        return deck;
    }

    int deckCount = 0;
    if (!tryParseInt(lines[0], deckCount) || deckCount < 0) {
        throw std::runtime_error("Invalid deck count line");
    }

    if (static_cast<int>(lines.size()) < deckCount + 1) {
        throw std::runtime_error("Insufficient deck lines");
    }

    for (int i = 0; i < deckCount; ++i) {
        deck.push_back(lines[i + 1]);
    }

    return deck;
}

std::vector<LogEntry>
StateParser::parseLog(const std::vector<std::string>& lines) const {
    std::vector<LogEntry> logs;
    if (lines.empty()) {
        return logs;
    }

    int logCount = 0;
    if (!tryParseInt(lines[0], logCount) || logCount < 0) {
        throw std::runtime_error("Invalid log count line");
    }

    if (static_cast<int>(lines.size()) < logCount + 1) {
        throw std::runtime_error("Insufficient log lines");
    }

    for (int i = 0; i < logCount; ++i) {
        const std::string& line = lines[i + 1];
        const std::vector<std::string> head = splitWhitespace(line);
        if (head.size() < 3) {
            throw std::runtime_error("Invalid log line: " + line);
        }

        int turn = 0;
        if (!tryParseInt(head[0], turn)) {
            throw std::runtime_error("Invalid log turn: " + line);
        }

        std::string detail;
        std::size_t p1 = line.find(' ');
        std::size_t p2 = (p1 == std::string::npos) ? std::string::npos
                                                   : line.find(' ', p1 + 1);
        std::size_t p3 = (p2 == std::string::npos) ? std::string::npos
                                                   : line.find(' ', p2 + 1);
        if (p3 != std::string::npos) {
            detail = line.substr(p3 + 1);
        }

        LogEntry entry;
        entry.turn = turn;
        entry.username = head[1];
        entry.actionType = head[2];
        entry.detail = detail;
        logs.push_back(entry);
    }

    return logs;
}
