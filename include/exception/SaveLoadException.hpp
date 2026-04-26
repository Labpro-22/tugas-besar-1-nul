#pragma once

#include <string>

#include "NimonspoliException.hpp"

class SaveLoadException : public NimonspoliException {
  public:
    explicit SaveLoadException(const std::string& message);
};
