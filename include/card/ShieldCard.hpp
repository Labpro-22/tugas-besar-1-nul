#include "SkillCard.hpp"
#pragma once
class ShieldCard : public SkillCard {
public:
    ShieldCard();

    void apply(TurnContext& ctx) override ;
};