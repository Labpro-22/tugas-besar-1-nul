#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class CommandException : public NimonspoliException {
public:
    explicit CommandException(const std::string& message);
};
