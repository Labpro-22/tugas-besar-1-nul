#include "exception/NimonspoliException.h"

NimonspoliException::NimonspoliException(const std::string& message)
    : std::runtime_error(message) {}
