#include "states/StateSerializer.hpp"

#include <sstream>

#include "utils/SerializeUtils.hpp"

std::string
StateSerializer::serializeHand(const std::vector<CardState> &hand) const {
    std::ostringstream out;
    for (std::size_t i = 0; i < hand.size(); ++i) {
        if (i > 0) {
            out << ';';
        }
        out << escapeField(hand[i].type) << ',' << hand[i].value << ','
            << hand[i].remainingDuration;
    }
    return out.str();
}

std::string StateSerializer::serializePlayers(
    const std::vector<PlayerState> &players) const {
    std::ostringstream out;
    for (const PlayerState &player : players) {
        out << escapeField(player.username) << '\t' << player.balance << '\t'
            << escapeField(player.positionCode) << '\t'
            << escapeField(player.status) << '\t' << serializeHand(player.hand)
            << '\n';
    }
    return out.str();
}

std::string StateSerializer::serializeProperties(
    const std::vector<PropertyState> &properties) const {
    std::ostringstream out;
    for (const PropertyState &property : properties) {
        out << escapeField(property.code) << '\t' << escapeField(property.type)
            << '\t' << escapeField(property.owner) << '\t'
            << escapeField(property.status) << '\t' << property.festivalMult
            << '\t' << property.festivalDur << '\t' << property.buildingCount
            << '\t' << (property.isHotel ? 1 : 0) << '\n';
    }
    return out.str();
}

std::string
StateSerializer::serializeDeck(const std::vector<std::string> &deck) const {
    std::ostringstream out;
    for (const std::string &card : deck) {
        out << escapeField(card) << '\n';
    }
    return out.str();
}

std::string
StateSerializer::serializeLog(const std::vector<LogEntry> &logs) const {
    std::ostringstream out;
    for (const LogEntry &entry : logs) {
        out << entry.turn << '\t' << escapeField(entry.username) << '\t'
            << escapeField(entry.actionType) << '\t'
            << escapeField(entry.detail) << '\n';
    }
    return out.str();
}
