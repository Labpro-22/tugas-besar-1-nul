#pragma once

#include <string>

#include "NimonspoliException.hpp"

class CardSlotFullException : public NimonspoliException {
  public:
    explicit CardSlotFullException(const std::string& message);
};
