#include "SkillCard.hpp"

class MoveCard : public SkillCard {
private:
    int steps;

public:
    MoveCard(int stp) ;

    void use(Player* player, TurnContext& ctx) override ;
};