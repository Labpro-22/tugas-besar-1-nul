#pragma once

#include <string>

class CardState {
  public:
    std::string type;
    int value = 0;
    int remainingDuration = 0;
};
