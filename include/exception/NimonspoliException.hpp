#pragma once

#include <stdexcept>
#include <string>

class NimonspoliException : public std::runtime_error {
  public:
    explicit NimonspoliException(const std::string& message);
    NimonspoliException(const std::string& type, const std::string& message);
};
