#pragma once

#include "SkillCard.hpp"
class LassoCard : public SkillCard {
public:
    LassoCard() ;

    void apply(TurnContext& ctx) override ;
};