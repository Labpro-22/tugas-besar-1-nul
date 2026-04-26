#pragma once

#include <string>
#include <vector>

#include "states/CardState.hpp"

class PlayerState {
  public:
    std::string username;
    int balance = 0;
    std::string positionCode;
    std::string status;
    bool isBot = false;  // Flag untuk menandai apakah player adalah bot
    std::vector<CardState> hand;
};
