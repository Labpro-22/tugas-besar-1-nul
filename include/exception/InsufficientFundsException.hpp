#pragma once

#include <string>

#include "exception/NimonspoliException.hpp"

class InsufficientFundsException : public NimonspoliException {
  public:
    explicit InsufficientFundsException(const std::string& message);
};
