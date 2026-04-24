#pragma once

#include <iostream>
#include <string>
#include <vector>

class TurnContext;

class Command {
private:
    mutable bool canEndTurn = false;

    std::string rawInput;
    std::vector<std::string> tokens;
    mutable std::vector<const char*> argvCache;

    void rebuildArgvCache() const;

    void execRollDice(TurnContext& ctx, std::ostream& out) const;
    void execSetDice(TurnContext& ctx, std::ostream& out) const;
    void execPrintCert(TurnContext& ctx, std::ostream& out) const;
    void execPrintProperty(TurnContext& ctx, std::ostream& out) const;
    void execProfile(TurnContext& ctx, std::ostream& out) const;
    void execMortgage(TurnContext& ctx, std::ostream& out) const;
    void execDismortgage(TurnContext& ctx, std::ostream& out) const;
    void execUpgrade(TurnContext& ctx, std::ostream& out) const;
    void execSave(TurnContext& ctx, std::ostream& out) const;
    void execLoad(TurnContext& ctx, std::ostream& out) const;
    void execPrintLog(TurnContext& ctx, std::ostream& out) const;
    void execUseSkill(TurnContext& ctx, std::ostream& out) const;
    void execHelp(std::ostream& out) const;
    void execPrintBoard(TurnContext& ctx, std::ostream& out) const;

  public:
    Command() = default;

    void promptInput(std::istream& in = std::cin,
                     std::ostream& out = std::cout);
    void parse(const std::string& input);

    int argc() const;
    const char* argv(int index) const;
    const std::vector<std::string>& args() const;

    // Single dispatcher containing all command conditionals.
    bool dispatch(TurnContext& ctx, std::ostream& out = std::cout) const;
};