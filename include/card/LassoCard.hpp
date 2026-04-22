#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "SkillCard.hpp"

class LassoCard : public SkillCard {
public:
    LassoCard();

    void apply(TurnContext& ctx) override;
};

#endif // LASSO_CARD_HPP
