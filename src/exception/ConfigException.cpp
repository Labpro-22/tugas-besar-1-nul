#include "exception/ConfigException.hpp"

ConfigException::ConfigException(const std::string& message)
    : NimonspoliException(message) {}
