#pragma once

#include "SkillCard.hpp"

class DiscountCard : public SkillCard {
private:
    int discountPercent;

public:
    DiscountCard(int discount) ;

    void apply(TurnContext& ctx) override ;
};
