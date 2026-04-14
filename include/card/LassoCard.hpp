#include "SkillCard.hpp"
class LassoCard : public SkillCard {
public:
    LassoCard() ;

    void use(Player* player, TurnContext& ctx) override ;
};