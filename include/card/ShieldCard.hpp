#pragma once

#include "SkillCard.hpp"

class ShieldCard : public SkillCard {
public:
    ShieldCard();

    void apply(TurnContext& ctx) override ;
};