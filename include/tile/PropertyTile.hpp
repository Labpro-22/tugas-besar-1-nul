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
        PropertyTile(int idx, Property *p) : Tile(idx, p->getCode(), p->getName()), property(p){};
        Property* getProperty();
        virtual void onLanded(TurnContext& ctx);
};

class StreetTile : public PropertyTile{
    // private:
    //     std::string colorCategory;
    public:
        StreetTile(int idx, StreetProperty *sp): PropertyTile(idx, sp){};
        void onLanded(TurnContext& ctx) override;
        void triggerBuyOrAuction(TurnContext& ctx);
        void triggerRentPayment(TurnContext& ctx);
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
