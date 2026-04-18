#include "SkillCard.hpp"
#pragma once
class DemolitionCard : public SkillCard {
public:
    DemolitionCard() ;

    void apply(TurnContext& ctx) override ;
};