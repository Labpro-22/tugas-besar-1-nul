#pragma once

#include <fstream>
#include <string>
#include <vector>

bool tryParseInt(const std::string &token, int &out);
std::vector<std::string> splitWhitespace(const std::string &line);
std::vector<std::string> readNextDataTokens(std::ifstream &in, int &lineNumber);
std::vector<int> parseNumericTail(const std::vector<std::string> &tokens,
                                  std::size_t startIndex);

std::vector<std::string> splitBy(const std::string &s, char delimiter);
bool parseBoolInt(const std::string &token);
