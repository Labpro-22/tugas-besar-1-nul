#include "SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
    DemolitionCard() ;

    void apply(Player& player) override ;
};