#pragma once

#include <vector>

class Player;
class Property;

class AuctionManager {
public:
    Player* runAuction(Property* prop, std::vector<Player*>& participants, int startingBid);
};