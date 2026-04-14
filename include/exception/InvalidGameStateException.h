#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class InvalidGameStateException : public NimonspoliException {
public:
    explicit InvalidGameStateException(const std::string& message);
};
