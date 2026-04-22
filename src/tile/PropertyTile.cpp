#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"
#include <iostream>

using namespace std;

PropertyTile::PropertyTile(int idx, string cd, string nm, string cat, Property* prop)
    : Tile(idx, cd, nm, cat), property(prop) {}

Property* PropertyTile::getProperty(){
    return property;
};

void PropertyTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik PropertyTile!\n";
};

StreetTile::StreetTile(int idx, string cd, string nm, string cat, Property* prop)
    : PropertyTile(idx, cd, nm, cat, prop) {}

void StreetTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik StreetTile!\n";
};

void StreetTile::triggerBuyOrAuction(Player* player){
    cout << "triggerBuyOrAuction milik StreetTile!\n";
};

void StreetTile::triggerRentPayment(Player* player){
    cout << "triggerRentPayment milik StreetTile!\n";
};

RailroadTile::RailroadTile(int idx, string cd, string nm, string cat, Property* prop)
    : PropertyTile(idx, cd, nm, cat, prop) {}

void RailroadTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik RailroadTile!\n";
}

void RailroadTile::autoAcquire(Player* player){
    cout << "autoAcquire milik RailroadTile!\n";
};

UtilityTile::UtilityTile(int idx, string cd, string nm, string cat, Property* prop)
    : PropertyTile(idx, cd, nm, cat, prop) {}

void UtilityTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik UtilityTile!\n";
}

void UtilityTile::autoAcquire(Player* player){
    cout << "autoAcquire milik UtilityTile!\n";
};
