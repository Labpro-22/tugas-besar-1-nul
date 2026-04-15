#pragma once

#include <string>

class LogEntry {
  public:
    int turn = 0;
    std::string username;
    std::string actionType;
    std::string detail;

    std::string toString() const;
};
