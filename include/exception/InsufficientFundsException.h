#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class InsufficientFundsException : public NimonspoliException {
public:
    explicit InsufficientFundsException(const std::string& message);
};
