#include "SkillCard.hpp"

class ShieldCard : public SkillCard {
public:
    ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

    void use(Player* player, TurnContext& ctx) override ;
};