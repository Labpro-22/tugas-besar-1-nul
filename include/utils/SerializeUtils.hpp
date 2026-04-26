#include <string>

std::string escapeField(const std::string& value);
std::string unescapeField(const std::string& value);
bool splitSerializedLine(const std::string& line,
                         std::string& turn,
                         std::string& username,
                         std::string& action,
                         std::string& detail);
