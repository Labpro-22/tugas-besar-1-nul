#pragma once

#include <string>
#include <vector>

#include "states/LogEntry.hpp"

class TransactionLogger {
  public:
    void log(int turn,
             const std::string& user,
             const std::string& action,
             const std::string& detail);
    std::vector<LogEntry> getAll() const;
    std::vector<LogEntry> getLast(int n) const;
    std::string serialize() const;
    void deserialize(const std::string& data);

  private:
    std::vector<LogEntry> entries;
};
