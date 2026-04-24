#pragma once

#include "SkillCard.hpp"

class TurnContext;

class MoveCard : public SkillCard {
private:
    int steps;

public:
    MoveCard(int stp);

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "MoveCard"; }
    int getSkillValue() const override { return steps; }
};
