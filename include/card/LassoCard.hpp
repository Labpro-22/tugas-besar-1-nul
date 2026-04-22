#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "SkillCard.hpp"

class LassoCard : public SkillCard {
public:
    LassoCard();

    void apply(Player& player) override;
};

#endif // LASSO_CARD_HPP
