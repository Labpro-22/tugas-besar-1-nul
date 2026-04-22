#include "SkillCard.hpp"
#pragma once
class DiscountCard : public SkillCard {
private:
    int discountPercent;

public:
    DiscountCard(int discount) ;

    void apply(TurnContext& ctx) override ;
};
