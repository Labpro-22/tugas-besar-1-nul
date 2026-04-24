#pragma once

#include "SkillCard.hpp"

class TurnContext;

class ShieldCard : public SkillCard {
  public:
    ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "ShieldCard"; }
};
