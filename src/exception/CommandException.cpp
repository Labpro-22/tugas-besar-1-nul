#include "exception/CommandException.hpp"

CommandException::CommandException(const std::string& message)
    : NimonspoliException(message) {}
