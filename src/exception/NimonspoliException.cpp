#include "exception/NimonspoliException.hpp"

namespace {
std::string formatExceptionMessage(const std::string& type,
                                   const std::string& message) {
    return message.empty() ? type : type + " " + message;
}
} // namespace

NimonspoliException::NimonspoliException(const std::string& message)
    : std::runtime_error(message) {}

NimonspoliException::NimonspoliException(const std::string& type,
                                         const std::string& message)
    : std::runtime_error(formatExceptionMessage(type, message)) {}
