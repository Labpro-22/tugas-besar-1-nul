#pragma once

#include "SkillCard.hpp"

class TurnContext;

class TeleportCard : public SkillCard {
private:
    int position;

public:
    TeleportCard(int pos);

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "TeleportCard"; }
    int getSkillValue() const override { return position; }
};
