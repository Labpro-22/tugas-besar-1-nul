#include "SkillCard.hpp"
class TeleportCard : public SkillCard {
public:
    TeleportCard();

    void use(Player* player, TurnContext& ctx) override ;
};