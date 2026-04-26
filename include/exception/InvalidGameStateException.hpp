#pragma once

#include <string>

#include "NimonspoliException.hpp"

class InvalidGameStateException : public NimonspoliException {
  public:
    explicit InvalidGameStateException(const std::string& message);
};
