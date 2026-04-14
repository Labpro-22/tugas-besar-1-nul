#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class CardSlotFullException : public NimonspoliException {
public:
    explicit CardSlotFullException(const std::string& message);
};
