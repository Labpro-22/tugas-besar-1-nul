#include "exception/InsufficientFundsException.hpp"

InsufficientFundsException::InsufficientFundsException(
    const std::string& message)
    : NimonspoliException(message) {}
