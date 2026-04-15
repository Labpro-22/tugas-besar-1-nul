#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class SaveLoadException : public NimonspoliException {
  public:
    explicit SaveLoadException(const std::string& message);
};
