#pragma once

#include <string>

#include "NimonspoliException.hpp"

class CommandException : public NimonspoliException {
  public:
    explicit CommandException(const std::string& message);
};
