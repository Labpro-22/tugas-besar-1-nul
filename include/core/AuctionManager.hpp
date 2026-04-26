#pragma once

#include <vector>

class Player;
class Property;

class AuctionWinner{
  public:
    Player& winner;
    Property& prop_won;
    int buyAmount;

    AuctionWinner(Player& player, Property& prop, int amount);
};

class AuctionManager {
  public:
    AuctionWinner runAuction(Property& prop,
                       const std::vector<Player*>& players,
                       int startIdx);

    int getLastWinningBid() const;

  private:
    int lastWinningBid_ = -1;
};
