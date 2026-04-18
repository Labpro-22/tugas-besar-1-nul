#include "SkillCard.hpp"

class ShieldCard : public SkillCard {
public:
    ShieldCard();

    void apply(Player& player) override ;
};