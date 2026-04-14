#include "SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
    DemolitionCard() ;

    void use(Player* player, TurnContext& ctx) override ;
};