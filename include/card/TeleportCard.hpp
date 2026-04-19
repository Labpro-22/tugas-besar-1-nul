#include "SkillCard.hpp"
class TeleportCard : public SkillCard {
private:
    int position;

public:
    TeleportCard(int pos);

    void apply(Player& player) override ;
};