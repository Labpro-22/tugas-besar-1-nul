#include "exception/InvalidGameStateException.h"

InvalidGameStateException::InvalidGameStateException(const std::string& message)
    : NimonspoliException(message) {}
