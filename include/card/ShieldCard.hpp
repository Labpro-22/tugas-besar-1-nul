#include "SkillCard.hpp"

class ShieldCard : public SkillCard {
public:
    ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

    void apply(Player& player) override ;
};