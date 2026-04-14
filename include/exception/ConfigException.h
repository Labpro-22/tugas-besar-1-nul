#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class ConfigException : public NimonspoliException {
public:
    explicit ConfigException(const std::string& message);
};
