#include "states/StateSerializer.hpp"

#include <sstream>

#include "states/CardState.hpp"

#include "utils/SerializeUtils.hpp"

std::string
StateSerializer::serializeHand(const std::vector<CardState>& hand) const {
    std::ostringstream out;
    out << hand.size() << '\n';

    for (const CardState& card : hand) {
        out << card.type;

        const bool hasValue = card.value != 0;
        const bool hasDuration = card.remainingDuration != 0;
        if (hasDuration || card.type == "DiscountCard") {
            out << ' ' << card.value << ' ' << card.remainingDuration;
        } else if (hasValue) {
            out << ' ' << card.value;
        }

        out << '\n';
    }

    return out.str();
}

std::string StateSerializer::serializePlayers(
    const std::vector<PlayerState>& players) const {
    std::ostringstream out;
    out << players.size() << '\n';

    for (const PlayerState& player : players) {
        out << player.username << ' ' << player.balance << ' '
            << player.positionCode << ' ' << player.status << '\n';
        out << serializeHand(player.hand);
    }

    return out.str();
}

std::string StateSerializer::serializeProperties(
    const std::vector<PropertyState>& properties) const {
    std::ostringstream out;
    out << properties.size() << '\n';

    for (const PropertyState& property : properties) {
        out << property.code << ' ' << property.type << ' ' << property.owner
            << ' ' << property.status << ' ' << property.festivalMult << ' '
            << property.festivalDur << ' '
            << (property.isHotel ? std::string("H")
                                 : std::to_string(property.buildingCount))
            << '\n';
    }

    return out.str();
}

std::string
StateSerializer::serializeDeck(const std::vector<std::string>& deck) const {
    std::ostringstream out;
    out << deck.size() << '\n';

    for (const std::string& card : deck) {
        out << card << '\n';
    }

    return out.str();
}

std::string
StateSerializer::serializeLog(const std::vector<LogEntry>& logs) const {
    std::ostringstream out;
    out << logs.size() << '\n';

    for (const LogEntry& entry : logs) {
        out << entry.turn << ' ' << entry.username << ' ' << entry.actionType;
        if (!entry.detail.empty()) {
            out << ' ' << entry.detail;
        }
        out << '\n';
    }

    return out.str();
}
