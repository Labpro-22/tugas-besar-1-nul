#include "states/LogEntry.hpp"

#include <iomanip>
#include <sstream>

std::string LogEntry::toString() const {
    std::ostringstream oss;
    oss << "[Turn " << turn << "] " << username << " | " << std::left
        << std::setw(8) << actionType << " | " << detail;
    return oss.str();
}
