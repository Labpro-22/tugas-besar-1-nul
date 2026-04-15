#include "SkillCard.hpp"
class DiscountCard : public SkillCard {
private:
    int discountPercent;

public:
    DiscountCard(int discount) ;

    void apply(Player& player) override ;
};
