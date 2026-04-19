#include "exception/SaveLoadException.hpp"

SaveLoadException::SaveLoadException(const std::string& message)
    : NimonspoliException(message) {}
