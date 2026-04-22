#ifndef MOVE_CARD_HPP
#define MOVE_CARD_HPP

#include "SkillCard.hpp"
#pragma once
class MoveCard : public SkillCard {
private:
    int steps;

public:
    MoveCard(int stp);

    void apply(Player& player) override;
};

#endif // MOVE_CARD_HPP
