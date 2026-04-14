#pragma once

#include <string>

#include "exception/NimonspoliException.h"

class SaveLoadException : public NimonspoliException {
public:
    explicit SaveLoadException(const std::string& message);
};
