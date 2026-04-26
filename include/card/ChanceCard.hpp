#pragma once

#include "card.hpp"

class Player;
class TurnContext;

enum class ChanceType { GO_TO_NEAREST_STATION, MOVE_BACK_3, GO_TO_JAIL };

class ChanceCard : public Card {
  private:
    ChanceType type;

  public:
    ChanceCard(std::string desc, ChanceType t);

    void execute(Player* player, TurnContext& ctx) override;
};
