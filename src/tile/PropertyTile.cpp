#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include <iostream>

using namespace std;

// semua konstruktor
PropertyTile::PropertyTile(int idx, Property &p) : Tile(idx, p.getCode(), p.getName()), property(&p){};

StreetTile::StreetTile(int idx, StreetProperty &prop) : PropertyTile(idx, prop){};
RailroadTile::RailroadTile(int idx, RailroadProperty &prop) : PropertyTile(idx, prop){};
UtilityTile::UtilityTile(int idx, UtilityProperty &prop) : PropertyTile(idx, prop){};

Property* PropertyTile::getProperty(){
    return property;
};

//helper
void printOwner(Player& player){
    cout << "Properti ini milik " << player.getUsername() << "\n";
}

void PropertyTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    printOwner(player);
}


void StreetTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::OWNED){
        if (property->getOwner() != &player){
            triggerRentPayment(ctx);
        }
    } else if (property->getStatus() == PropertyStatus::BANK){
        triggerBuyOrAuction(ctx);
    } //kalau Mortgaged lewat aja sih
    
}

void StreetTile::triggerBuyOrAuction(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "[" << player.getUsername() << "] mendarat di [" << getName() << "].\n\n";
    getProperty()->printStatus(ctx);
    string ans;
    while (true){
        cout << "[Y/N] Apakah Anda mau beli " << getName() << "? (Harga: " << getProperty()->getBuyPrice() << ")\n\n";
        cin >> ans;
        if (ans == "Y" || ans == "y"){
            player.buy(getProperty());
            cout << "[" << player.getUsername() << "] baru saja membeli " << getName() << "\n";
            cout << "Uang [" << player.getUsername() << "] tersisa: " << player.getBalance() - getProperty()->getBuyPrice() << "\n\n"; //nanti implement dari player, biar bisa kurangi balance player
            break;
        } else if (ans == "N" || ans == "n"){
            cout << "AUCTIONNNNNNNNN\n"; //nanti masukkan fungsi auction
            break;
        } else{
            cout << "input tidak valid. Throw input invalid exception.\n";
        }
    }
}

void StreetTile::triggerRentPayment(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "[" << player.getUsername() << "] mendarat di [" << getProperty()->getName() << "] milik [" << getProperty()->getOwner()->getUsername() << "].\n\n";
    getProperty()->printStatus(ctx);
    cout << "Uang anda tersisa: <M" << player.getBalance() - getProperty()->getRent(ctx) << ">.\n\n"; //nanti implement dari player, biar bisa kurangi balance player
}

void RailroadTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::BANK){
        autoAcquire(player);
    } else if (property->getStatus() == PropertyStatus::OWNED){
        
        if (property->getOwner() != &player){
            Property* prop = getProperty();
            cout << "[" << player.getUsername() << "] mendarat di [" << prop->getName() << "] milik [" << prop->getOwner()->getUsername() << "].\n\n";
            prop->printStatus(ctx); //ini harusnya jadi 0 harga belinya
            cout << "Uang anda tersisa: <M" << player.getBalance() - prop->getRent(ctx) << ">.\n\n"; //nanti implement dari player, biar bisa kurangi balance player
        }
    } else{
        //kalau Mortgaged lewat aja sih
    }    
}

void RailroadTile::autoAcquire(Player& player){
    Property* prop = getProperty();
    player.addProperty(prop);
    cout << "[" << prop->getName() << "] menjadi milik [" << prop->getOwner()->getUsername() << "].\n\n";
}

void UtilityTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::BANK){
        autoAcquire(player);
        
    } else if (property->getStatus() == PropertyStatus::OWNED){
        
        if (property->getOwner() != &player){
            Property* prop = getProperty();
            cout << "[" << player.getUsername() << "] mendarat di [" << prop->getName() << "] milik [" << prop->getOwner()->getUsername() << "].\n\n";
            prop->printStatus(ctx); //ini harusnya jadi 0 harga belinya
            cout << "Uang anda tersisa: <M" << player.getBalance() << ">.\n\n"; //nanti implement dari player
        }
    } else{
        //kalau Mortgaged lewat aja sih
    }   
}

void UtilityTile::autoAcquire(Player& player){
    Property* prop = getProperty();
    player.addProperty(prop);
    cout << "[" << prop->getName() << "] menjadi milik [" << prop->getOwner()->getUsername() << "].\n\n";
}