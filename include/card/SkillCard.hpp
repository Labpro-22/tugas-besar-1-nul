#pragma once

#include "card.hpp"

class Player;
class TurnContext;

class SkillCard : public Card {
protected:
    int remainingDuration; 

public:
    SkillCard(std::string desc, int duration);

    int getDuration() const;

    void decreaseDuration();

    void execute(Player* player, TurnContext& ctx) override;

    virtual void apply(TurnContext& ctx) = 0;

    virtual std::string getTypeName() const = 0;
    virtual int getSkillValue() const { return 0; }
    int getRemainingDuration() const { return remainingDuration; }
};

