#include "SkillCard.hpp"
#pragma once
class MoveCard : public SkillCard {
private:
    int steps;

public:
    MoveCard(int stp) ;

    void apply(TurnContext& ctx) override ;
};