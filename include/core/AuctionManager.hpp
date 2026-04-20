#pragma once

#include <vector>

class Player;
class Property;

class AuctionManager {
  public:
    // Runs auction from the player after startIdx and returns winner or
    // nullptr.
    Player* runAuction(Property* prop,
                       const std::vector<Player*>& players,
                       int startIdx);

    int getLastWinningBid() const;

  private:
    int lastWinningBid_ = -1;
};
