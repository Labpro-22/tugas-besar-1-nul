#pragma once

#include "core/TurnContext.hpp"
#include "core/Dice.hpp"
#include "board/Board.hpp" //nanti dihapus kalau ga dibutuhkan
#include "property/Property.hpp"
#include "player/Player.h"
#include "Tile.hpp"
#include <iostream>
#include <vector>


// ======================================================================================

class PropertyTile : public Tile{
    protected: 
        Property* property;
    public:
        PropertyTile(int idx, string cd, string nm, string cat, Property* p) : Tile(idx, cd, nm, cat) , property(p){};
        Property* getProperty();
        virtual void onLanded(Player* player, TurnContext& ctx);
};



class StreetTile : public PropertyTile{
    public:
        using PropertyTile::PropertyTile;
        void onLanded(Player* player, TurnContext& ctx) override;
        void triggerBuyOrAuction(Player* player, TurnContext& ctx);
        void triggerRentPayment(Player* player);
};

class RailroadTile : public PropertyTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void autoAcquire(Player* player);
};

class UtilityTile : public PropertyTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void autoAcquire(Player* player); 
};

//================================= HELPER ================================
void printOwner(Player* player);
