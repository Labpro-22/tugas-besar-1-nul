#ifndef PROPERTY_TILE_HPP
#define PROPERTY_TILE_HPP

#pragma once

#include "../board/Board.hpp" //nanti dihapus kalau ga dibutuhkan
#include "../property/Property.hpp"
#include "property/StreetProperty.hpp"
#include "../player/Player.h"
#include "core/Dice.hpp"
#include "Tile.hpp"
#include <iostream>
#include <vector>

// =================================== dummy classes =====================================

class Dice;

class TurnContext;

class StreetProperty;


// ======================================================================================

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
        void onLanded(TurnContext& ctx) override;
        void autoAcquire(Player* player);
};

class UtilityTile : public PropertyTile{
    public:
        void onLanded(TurnContext& ctx) override;
        void autoAcquire(Player* player); 
};

//================================= HELPER ================================
void printOwner(Player* player);


#endif