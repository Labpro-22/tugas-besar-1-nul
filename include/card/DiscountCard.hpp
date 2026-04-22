#ifndef DISCOUNT_CARD_HPP
#define DISCOUNT_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class DiscountCard : public SkillCard {
private:
    int discountPercent;

public:
    DiscountCard(int discount);

    void apply(Player& player) override;
};

#endif // DISCOUNT_CARD_HPP
