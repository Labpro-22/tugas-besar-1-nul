#include "exception/CardSlotFullException.hpp"

CardSlotFullException::CardSlotFullException(const std::string& message)
    : NimonspoliException(message) {}
