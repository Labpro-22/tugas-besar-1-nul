#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();

    void apply(Player& player) override;
};

#endif // DEMOLITION_CARD_HPP
