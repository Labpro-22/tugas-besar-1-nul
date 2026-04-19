#include "exception/NimonspoliException.hpp"

NimonspoliException::NimonspoliException(const std::string& message)
    : std::runtime_error(message) {}
