#ifndef SHIELD_CARD_HPP
#define SHIELD_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class ShieldCard : public SkillCard {
public:
    ShieldCard();

    void apply(Player& player) override;
};

#endif // SHIELD_CARD_HPP
