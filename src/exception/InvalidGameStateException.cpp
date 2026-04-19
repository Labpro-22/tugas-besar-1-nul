#include "exception/InvalidGameStateException.hpp"

InvalidGameStateException::InvalidGameStateException(const std::string& message)
    : NimonspoliException(message) {}
