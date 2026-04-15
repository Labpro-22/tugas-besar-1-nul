#pragma once

#include <string>
#include <vector>

#include "states/GameState.hpp"
#include "states/LogEntry.hpp"
#include "states/PlayerState.hpp"
#include "states/PropertyState.hpp"

class SaveLoadManager {
  public:
    void save(const GameState& state, const std::string& filename) const;
    GameState load(const std::string& filename) const;

  private:
    std::string serializePlayers(const std::vector<PlayerState>& players) const;
    std::string
    serializeProperties(const std::vector<PropertyState>& properties) const;
    std::string serializeDeck(const std::vector<std::string>& deck) const;
    std::string serializeLog(const std::vector<LogEntry>& logs) const;

    std::vector<PlayerState>
    parsePlayers(const std::vector<std::string>& lines) const;
    std::vector<PropertyState>
    parseProperties(const std::vector<std::string>& lines) const;
    std::vector<std::string>
    parseDeck(const std::vector<std::string>& lines) const;
    std::vector<LogEntry> parseLog(const std::vector<std::string>& lines) const;
};
