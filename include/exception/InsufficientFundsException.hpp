#pragma once

#include <string>

#include "NimonspoliException.hpp"

class InsufficientFundsException : public NimonspoliException {
  public:
    explicit InsufficientFundsException(const std::string& message);
};
