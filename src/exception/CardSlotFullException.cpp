#include "exception/CardSlotFullException.h"

CardSlotFullException::CardSlotFullException(const std::string& message)
    : NimonspoliException(message) {}
