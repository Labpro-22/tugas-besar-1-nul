#pragma once

#include "card.hpp"

class Player;

class SkillCard : public Card {
protected:
    int remainingDuration; 

public:
    SkillCard(std::string desc, int duration);

    int getDuration() const ;

    void decreaseDuration() ;

    void execute(Player* player, TurnContext& ctx) override ;

    virtual void apply(TurnContext& ctx) = 0;
};