#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class InvalidGameStateException : public NimonspoliException {
  public:
    explicit InvalidGameStateException(const std::string& message);
};
