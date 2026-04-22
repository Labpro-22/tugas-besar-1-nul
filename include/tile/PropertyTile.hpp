#ifndef PROPERTY_TILE_HPP
#define PROPERTY_TILE_HPP

#pragma once

#include "Tile.hpp"
#include <iostream>
#include <vector>

// Forward declarations
class Player;
class TurnContext;
class Property;

class PropertyTile : public Tile{
    protected: 
        Property* property;
    public:
        PropertyTile(int idx, string cd, string nm, string cat, Property* prop);
        Property* getProperty();
        virtual void onLanded(Player* player, TurnContext& ctx);
};

class StreetTile : public PropertyTile{
    public:
        StreetTile(int idx, string cd, string nm, string cat, Property* prop);
        void onLanded(Player* player, TurnContext& ctx) override;
        void triggerBuyOrAuction(Player* player);
        void triggerRentPayment(Player* player);
};

class RailroadTile : public PropertyTile{
    public:
        RailroadTile(int idx, string cd, string nm, string cat, Property* prop);
        void onLanded(Player* player, TurnContext& ctx) override;
        void autoAcquire(Player* player);
};

class UtilityTile : public PropertyTile{
    public:
        UtilityTile(int idx, string cd, string nm, string cat, Property* prop);
        void onLanded(Player* player, TurnContext& ctx) override;
        void autoAcquire(Player* player); 
};

#endif
