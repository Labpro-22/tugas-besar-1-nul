#include "exception/BankruptcyException.hpp"

BankruptcyException::BankruptcyException(const std::string& message)
    : NimonspoliException(message) {}
