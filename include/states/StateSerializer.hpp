#pragma once

#include <string>
#include <vector>

#include "states/LogEntry.hpp"
#include "states/PlayerState.hpp"
#include "states/PropertyState.hpp"

class StateSerializer {
  public:
    std::string serializePlayers(const std::vector<PlayerState> &players) const;
    std::string
    serializeProperties(const std::vector<PropertyState> &properties) const;
    std::string serializeDeck(const std::vector<std::string> &deck) const;
    std::string serializeLog(const std::vector<LogEntry> &logs) const;
    std::string serializeHand(const std::vector<CardState> &hand) const;
};
