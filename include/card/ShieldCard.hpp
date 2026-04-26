#pragma once

#include "SkillCard.hpp"

class TurnContext;

class ShieldCard : public SkillCard {
  public:
    ShieldCard() : SkillCard("|Shield Card|: Immunity to bad effects for the current turn", 1) {}

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "ShieldCard"; }
};
