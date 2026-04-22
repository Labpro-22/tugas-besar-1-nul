#ifndef SHIELD_CARD_HPP
#define SHIELD_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class ShieldCard : public SkillCard {
public:
    ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

    void apply(TurnContext& ctx) override;
};

#endif // SHIELD_CARD_HPP
