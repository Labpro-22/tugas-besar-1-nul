#include "tile/PropertyTile.hpp"

Property* PropertyTile::getProperty(){
    return property;
};

//helper
void printOwner(Player* player){
    cout << "Properti ini milik " << player->getUsername() << "\n";
}

void PropertyTile::onLanded(Player* player, TurnContext& ctx){
    printOwner(player);
};

void StreetTile::onLanded(Player* player, TurnContext& ctx){
    if (property->getStatus() == PropertyStatus::OWNED){
        if (property->getOwner() == player){
        } else{
            triggerRentPayment(player, ctx);
        }
    } else if (property->getStatus() == PropertyStatus::BANK){
        triggerBuyOrAuction(player, ctx);
    } else{
        //kalau Mortgaged lewat aja sih
    }
};

void StreetTile::triggerBuyOrAuction(Player* player, TurnContext& ctx){
    cout << "Anda mendarat di " << getName() << ".\n";
    getProperty()->printStatus(ctx);
    string ans;
    while (true){
        cout << "[Y/N] Apakah Anda mau beli " << getName() << "? (Harga: " << getProperty()->getBuyPrice() << ")\n";
        cin >> ans;
        if (ans == "Y" || ans == "y"){
            player->buy(getProperty());
            cout << "Anda baru saja membeli " << getName() << "\n";
            cout << "Uang anda tersisa: " << player->getBalance() - getProperty()->getBuyPrice() << "\n"; //nanti implement dari player
            break;
        } else if (ans == "N" || ans == "n"){
            cout << "AUCTIONNNNNNNNN\n";
            break;
        } else{
            cout << "input tidak valid. Throw input invalid exception?\n";
        }
    }
};

void StreetTile::triggerRentPayment(Player* player, TurnContext& ctx){
    cout << "Anda mendarat di " << getProperty()->getName() << " milik " << getProperty()->getOwner()->getUsername() << "\n";
    getProperty()->printStatus(ctx);
    cout << "Uang anda tersisa: " << player->getBalance() - getProperty()->getRent(ctx) << "\n"; //nanti implement dari player
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