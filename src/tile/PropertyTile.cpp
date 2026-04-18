#include "tile/PropertyTile.hpp"

Property* PropertyTile::getProperty(){
    return property;
};

//helper
void printOwner(Player* player){
    cout << "Properti ini milik " << player->getUsername() << "\n";
}

void PropertyTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik PropertyTile!\n";
    printOwner(player);
};

void StreetTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik StreetTile!\n";
    if (property->getStatus() == PropertyStatus::OWNED){
        if (property->getOwner() == player){
            cout << "Welcome home, brader\n";
        } else{
            cout << "Nah loh ke tempat siapa ni\n";
            triggerRentPayment(player);
        }
    } else if (property->getStatus() == PropertyStatus::BANK){
        triggerBuyOrAuction(player, ctx);
    } else{
        //kalau Mortgaged lewat aja sih
    }
};

void StreetTile::triggerBuyOrAuction(Player* player, TurnContext& ctx){
    cout << "triggerBuyOrAuction milik StreetTile!\n";
    cout << "Anda mendarat di " << getName() << ".\n";
    getProperty()->printStatus(ctx);
    cout << "Apakah Anda mau beli " << getName() << "? (Harga: " << getProperty()->getBuyPrice() << ")\n";
    string ans;
    cin >> ans;
};

void StreetTile::triggerRentPayment(Player* player){
    cout << "triggerRentPayment milik StreetTile!\n";
    cout << "Anda mendarat di " << property->getName() << " milik " << property->getOwner()->getUsername() << "\n";
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