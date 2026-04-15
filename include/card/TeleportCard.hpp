#include "SkillCard.hpp"
class TeleportCard : public SkillCard {
private:
    int position;

public:
    TeleportCard();

    void apply(Player& player) override ;
};