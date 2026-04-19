#pragma once

#include <string>
#include <vector>

#include "states/LogEntry.hpp"
#include "states/PlayerState.hpp"
#include "states/PropertyState.hpp"

class StateParser {
  public:
    std::vector<CardState> parseHand(const std::string& serializedHand) const;
    std::vector<PlayerState>
    parsePlayers(const std::vector<std::string>& lines) const;
    std::vector<PropertyState>
    parseProperties(const std::vector<std::string>& lines) const;
    std::vector<std::string>
    parseDeck(const std::vector<std::string>& lines) const;
    std::vector<LogEntry> parseLog(const std::vector<std::string>& lines) const;
};
