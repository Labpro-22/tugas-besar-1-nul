#include "exception/CommandException.h"

CommandException::CommandException(const std::string& message)
    : NimonspoliException(message) {}
