#include "exception/ConfigException.h"

ConfigException::ConfigException(const std::string& message)
    : NimonspoliException(message) {}
