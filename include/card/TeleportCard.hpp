#ifndef TELEPORT_CARD_HPP
#define TELEPORT_CARD_HPP

#include "SkillCard.hpp"

class TeleportCard : public SkillCard {
private:
    int position;

public:
    TeleportCard(int pos);

    void apply(TurnContext& ctx) override;
};

#endif // TELEPORT_CARD_HPP
