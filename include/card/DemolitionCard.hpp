#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class DemolitionCard : public SkillCard {
public:
    DemolitionCard();

    void apply(Player& player) override;
};

#endif // DEMOLITION_CARD_HPP
