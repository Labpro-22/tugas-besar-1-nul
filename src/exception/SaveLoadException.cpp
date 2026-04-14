#include "exception/SaveLoadException.h"

SaveLoadException::SaveLoadException(const std::string& message)
    : NimonspoliException(message) {}
