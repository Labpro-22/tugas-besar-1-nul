#pragma once

#include <iostream>
#include <string>
#include <vector>

class TurnContext;

class Command {
private:
    std::string rawInput;
    std::vector<std::string> tokens;
    mutable std::vector<const char*> argvCache;

    void rebuildArgvCache() const;

public:
    Command() = default;

    void promptInput(std::istream& in = std::cin, std::ostream& out = std::cout);
    void parse(const std::string& input);

    int argc() const;
    const char* argv(int index) const;
    const std::vector<std::string>& args() const;

    // Single dispatcher containing all command conditionals.
    bool dispatch(TurnContext& ctx, std::ostream& out = std::cout) const;
};