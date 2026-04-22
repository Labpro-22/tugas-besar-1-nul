#ifndef CHANCE_CARD_HPP
#define CHANCE_CARD_HPP

#include "card.hpp"

enum class ChanceType {
    GO_TO_NEAREST_STATION,
    MOVE_BACK_3,
    GO_TO_JAIL
};

class ChanceCard : public Card {
private:
    ChanceType type;

public:
    ChanceCard(std::string desc, ChanceType t);

    void execute(Player* player, TurnContext& ctx) override;
};

#endif // CHANCE_CARD_HPP
