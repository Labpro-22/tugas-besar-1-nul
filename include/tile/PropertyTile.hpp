#ifndef PROPERTY_TILE_HPP
#define PROPERTY_TILE_HPP

#pragma once

#include "../board/Board.hpp" //nanti dihapus kalau ga dibutuhkan
#include "Tile.hpp"
#include <iostream>
#include <vector>


// =================================== dummy classes =====================================

class Dice{
    //dummy data
};

class Player{
    //dummy data
};

class TurnContext{
    //dummy data
};

class Property{
    public: 
        int getRent(TurnContext& ctx);
};

// ======================================================================================

class PropertyTile : public Tile{
    protected: 
        Property* property;
    public:
        Property* getProperty();
        virtual void onLanded(Player* player, TurnContext& ctx);
};



class StreetTile : public PropertyTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void triggerBuyOrAuction(Player* player);
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

#endif