#include "exception/InsufficientFundsException.h"

InsufficientFundsException::InsufficientFundsException(const std::string& message)
    : NimonspoliException(message) {}
