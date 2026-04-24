#pragma once

#include "SkillCard.hpp"

class TurnContext;

class DiscountCard : public SkillCard {
  private:
    int discountPercent;

  public:
    DiscountCard(int discount);

    void apply(TurnContext& ctx) override;
    std::string getTypeName() const override { return "DiscountCard"; }
    int getSkillValue() const override { return discountPercent; }
};
