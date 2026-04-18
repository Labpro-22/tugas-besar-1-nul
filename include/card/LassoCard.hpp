#include "SkillCard.hpp"
#pragma once
class LassoCard : public SkillCard {
public:
    LassoCard() ;

    void apply(TurnContext& ctx) override ;
};