#include "SkillCard.hpp"
#pragma once
class TeleportCard : public SkillCard {
private:
    int position;

public:
    TeleportCard(int pos);

    void apply(TurnContext& ctx) override ;
};