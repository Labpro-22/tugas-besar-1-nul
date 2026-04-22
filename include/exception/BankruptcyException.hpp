#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class BankruptcyException : public NimonspoliException {
  public:
    explicit BankruptcyException(const std::string& message);
};
