#pragma once

#include <string>
#include <vector>

#include "states/LogEntry.hpp"
#include "states/PlayerState.hpp"
#include "states/PropertyState.hpp"

class GameState {
  public:
    int currentTurn = 0;
    int maxTurn = 0;
    std::vector<PlayerState> players;
    std::vector<int> turnOrder;
    int activePlayerIdx = 0;
    std::vector<PropertyState> properties;
    std::vector<std::string> deckState;
    std::vector<LogEntry> log;
};
