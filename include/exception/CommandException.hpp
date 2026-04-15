#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class CommandException : public NimonspoliException {
  public:
    explicit CommandException(const std::string& message);
};
