#pragma once

#include "SkillCard.hpp"

class TurnContext;

class LassoCard : public SkillCard {
public:
    LassoCard();

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "LassoCard"; }
};
