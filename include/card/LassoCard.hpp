#include "SkillCard.hpp"
class LassoCard : public SkillCard {
public:
    LassoCard() ;

    void apply(Player& player) override ;
};