#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class DemolitionCard : public SkillCard {
public:
    DemolitionCard();

    void apply(TurnContext& ctx) override;
};

#endif // DEMOLITION_CARD_HPP
