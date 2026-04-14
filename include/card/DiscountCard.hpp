#include "SkillCard.hpp"
class DiscountCard : public SkillCard {
private:
    int discountPercent;

public:
    DiscountCard(int discount) ;

    void use(Player* player, TurnContext& ctx) override ;
};
