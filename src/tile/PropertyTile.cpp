#include "tile/PropertyTile.hpp"
#include "core/TurnContext.hpp"

Property* PropertyTile::getProperty(){
    return property;
};

//helper
void printOwner(Player* player){
    cout << "Properti ini milik " << player->getUsername() << "\n";
}

void PropertyTile::onLanded(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    printOwner(player);
};

void StreetTile::onLanded(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::OWNED){
        if (property->getOwner() == player){
        } else{
            triggerRentPayment(ctx);
        }
    } else if (property->getStatus() == PropertyStatus::BANK){
        triggerBuyOrAuction(ctx);
    } else{
        //kalau Mortgaged lewat aja sih
    }
};

void StreetTile::triggerBuyOrAuction(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    cout << "Anda mendarat di [" << getName() << "].\n\n";
    getProperty()->printStatus(ctx);
    string ans;
    while (true){
        cout << "[Y/N] Apakah Anda mau beli " << getName() << "? (Harga: " << getProperty()->getBuyPrice() << ")\n\n";
        cin >> ans;
        if (ans == "Y" || ans == "y"){
            player->buy(getProperty());
            cout << "Anda baru saja membeli " << getName() << "\n";
            cout << "Uang anda tersisa: " << player->getBalance() - getProperty()->getBuyPrice() << "\n\n"; //nanti implement dari player
            break;
        } else if (ans == "N" || ans == "n"){
            cout << "AUCTIONNNNNNNNN\n"; //nanti masukkan fungsi auction
            break;
        } else{
            cout << "input tidak valid. Throw input invalid exception.\n";
        }
    }
};

void StreetTile::triggerRentPayment(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    cout << "Anda mendarat di [" << getProperty()->getName() << "] milik [" << getProperty()->getOwner()->getUsername() << "].\n\n";
    getProperty()->printStatus(ctx);
    cout << "Uang anda tersisa: <M" << player->getBalance() - getProperty()->getRent(ctx) << ">.\n\n"; //nanti implement dari player
};

void RailroadTile::onLanded(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    cout << "onLanded milik RailroadTile!\n";
}

void RailroadTile::autoAcquire(Player* player){
    cout << "autoAcquire milik RailroadTile!\n";
};

void UtilityTile::onLanded(TurnContext& ctx){
    Player* player = ctx.currentPlayer;
    cout << "onLanded milik UtilityTile!\n";
}

void UtilityTile::autoAcquire(Player* player){
    cout << "autoAcquire milik UtilityTile!\n";
};