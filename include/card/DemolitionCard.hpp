#include "SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
    DemolitionCard() ;

    void apply(TurnContext& ctx) override ;
};