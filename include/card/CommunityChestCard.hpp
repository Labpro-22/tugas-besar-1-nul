#pragma once

#include "card.hpp"

class Player;
class TurnContext;

enum class CommunityChestType {
    BIRTHDAY,      // Get M100 from each player
    DOCTOR_FEE,    // Pay M700
    POLITICAL_CAMPAIGN  // Pay M200 to each player
};

class CommunityChestCard : public Card {
private:
    CommunityChestType type;
    int amount;

public:
    CommunityChestCard(std::string desc, CommunityChestType t, int amt);

    void execute(Player* player, TurnContext& ctx) override;
    CommunityChestType getType() const;
    int getAmount() const;
};
