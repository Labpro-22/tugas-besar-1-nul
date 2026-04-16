#include "../../include/tile/PropertyTile.hpp"

Property* PropertyTile::getProperty(){
    return property;
};

void PropertyTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik PropertyTile!\n";
};

void StreetTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik StreetTile!\n";
};

void StreetTile::triggerBuyOrAuction(Player* player){
    cout << "triggerBuyOrAuction milik StreetTile!\n";
};

void StreetTile::triggerRentPayment(Player* player){
    cout << "triggerRentPayment milik StreetTile!\n";
};

void RailroadTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik RailroadTile!\n";
}

void RailroadTile::autoAcquire(Player* player){
    cout << "autoAcquire milik RailroadTile!\n";
};

void UtilityTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik UtilityTile!\n";
}

void UtilityTile::autoAcquire(Player* player){
    cout << "autoAcquire milik UtilityTile!\n";
};