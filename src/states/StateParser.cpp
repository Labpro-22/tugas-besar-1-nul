#include "states/StateParser.hpp"

#include <sstream>
#include <stdexcept>

#include "utils/ParseUtils.hpp"
#include "utils/SerializeUtils.hpp"

std::vector<CardState>
StateParser::parseHand(const std::string &serializedHand) const {
    std::vector<CardState> hand;
    if (serializedHand.empty()) {
        return hand;
    }

    const std::vector<std::string> cards = splitBy(serializedHand, ';');
    for (const std::string &cardRaw : cards) {
        if (cardRaw.empty()) {
            continue;
        }

        const std::vector<std::string> parts = splitBy(cardRaw, ',');
        if (parts.size() != 3) {
            throw std::runtime_error("Invalid serialized card entry: " +
                                     cardRaw);
        }

        int value = 0;
        int duration = 0;
        if (!tryParseInt(parts[1], value) || !tryParseInt(parts[2], duration)) {
            throw std::runtime_error("Invalid card numeric fields: " + cardRaw);
        }

        CardState card;
        card.type = unescapeField(parts[0]);
        card.value = value;
        card.remainingDuration = duration;
        hand.push_back(card);
    }

    return hand;
}
std::vector<PlayerState>
StateParser::parsePlayers(const std::vector<std::string> &lines) const {
    std::vector<PlayerState> players;

    for (const std::string &line : lines) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> fields = splitBy(line, '\t');
        if (fields.size() < 5) {
            throw std::runtime_error("Invalid player line: " + line);
        }

        int balance = 0;
        if (!tryParseInt(fields[1], balance)) {
            throw std::runtime_error("Invalid player balance: " + line);
        }

        PlayerState player;
        player.username = unescapeField(fields[0]);
        player.balance = balance;
        player.positionCode = unescapeField(fields[2]);
        player.status = unescapeField(fields[3]);
        player.hand = parseHand(fields[4]);

        players.push_back(player);
    }

    return players;
}

std::vector<PropertyState>
StateParser::parseProperties(const std::vector<std::string> &lines) const {
    std::vector<PropertyState> properties;

    for (const std::string &line : lines) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> fields = splitBy(line, '\t');
        if (fields.size() < 8) {
            throw std::runtime_error("Invalid property line: " + line);
        }

        int festivalMult = 0;
        int festivalDur = 0;
        int buildingCount = 0;
        if (!tryParseInt(fields[4], festivalMult) ||
            !tryParseInt(fields[5], festivalDur) ||
            !tryParseInt(fields[6], buildingCount)) {
            throw std::runtime_error("Invalid property numeric fields: " +
                                     line);
        }

        PropertyState property;
        property.code = unescapeField(fields[0]);
        property.type = unescapeField(fields[1]);
        property.owner = unescapeField(fields[2]);
        property.status = unescapeField(fields[3]);
        property.festivalMult = festivalMult;
        property.festivalDur = festivalDur;
        property.buildingCount = buildingCount;
        property.isHotel = parseBoolInt(fields[7]);

        properties.push_back(property);
    }

    return properties;
}

std::vector<std::string>
StateParser::parseDeck(const std::vector<std::string> &lines) const {
    std::vector<std::string> deck;
    for (const std::string &line : lines) {
        if (line.empty()) {
            continue;
        }
        deck.push_back(unescapeField(line));
    }
    return deck;
}

std::vector<LogEntry>
StateParser::parseLog(const std::vector<std::string> &lines) const {
    std::vector<LogEntry> logs;

    for (const std::string &line : lines) {
        if (line.empty()) {
            continue;
        }

        std::string turnRaw;
        std::string usernameRaw;
        std::string actionRaw;
        std::string detailRaw;
        if (!splitSerializedLine(line, turnRaw, usernameRaw, actionRaw,
                                 detailRaw)) {
            throw std::runtime_error("Invalid log line: " + line);
        }

        int turn = 0;
        if (!tryParseInt(turnRaw, turn)) {
            throw std::runtime_error("Invalid log turn: " + line);
        }

        LogEntry entry;
        entry.turn = turn;
        entry.username = unescapeField(usernameRaw);
        entry.actionType = unescapeField(actionRaw);
        entry.detail = unescapeField(detailRaw);
        logs.push_back(entry);
    }

    return logs;
}
