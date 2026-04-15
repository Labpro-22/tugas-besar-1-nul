#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class ConfigException : public NimonspoliException {
  public:
    explicit ConfigException(const std::string& message);
};
