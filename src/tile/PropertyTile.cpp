#include "tile/PropertyTile.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "core/BankruptcyManager.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"
#include "gui/GuiPanelManager.hpp"
#include <iostream>
#include <limits>
#include "core/TurnManager.hpp"
#include "core/AuctionManager.hpp"

using namespace std;

// semua konstruktor
PropertyTile::PropertyTile(int idx, Property& p)
    : Tile(idx, p.getCode(), p.getName()), property(&p) {};

StreetTile::StreetTile(int idx, StreetProperty& prop)
    : PropertyTile(idx, prop) {};
RailroadTile::RailroadTile(int idx, RailroadProperty& prop)
    : PropertyTile(idx, prop) {};
UtilityTile::UtilityTile(int idx, UtilityProperty& prop)
    : PropertyTile(idx, prop) {};

Property* PropertyTile::getProperty() {
    return property;
};

// helper
void printOwner(Player& player) {
    cout << "Properti ini milik " << player.getUsername() << "\n";
}

void PropertyTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    printOwner(player);
}

void StreetTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::OWNED) {
        if (property->getOwner() != &player) {
            triggerRentPayment(ctx);
        }
    } else if (property->getStatus() == PropertyStatus::BANK) {
        triggerBuyOrAuction(ctx);
    } // kalau Mortgaged lewat aja sih
}

void StreetTile::triggerBuyOrAuction(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    cout << "[" << player.getUsername() << "] landed on [" << getName()
         << "].\n\n";
    getProperty()->printStatus(ctx);

    // GUI mode: show buy panel (only for human players)
    if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
        ctx.gameEngine.getPanelManager()->showPropertyBuy(player, *getProperty(), ctx);
        return;
    }

    // Bot players skip CLI input and do not buy
    if (player.isBot()) {
        return;
    }

    string ans;
    while (true) {
        cout << "[Y/N] Apakah Anda mau beli " << getName()
             << "? (Harga: " << getProperty()->getBuyPrice() << ")\n\n";
        cin >> ans;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear newline from buffer
        if (ans == "Y" || ans == "y"){
            player.buy(getProperty(), ctx);
            cout << "[" << player.getUsername() << "] baru saja membeli " << getName() << "\n";
            cout << "Uang [" << player.getUsername() << "] tersisa: " << player.getBalance() << "\n\n";
            break;
        } else if (ans == "N" || ans == "n"){
            AuctionManager am;
            TurnManager tm = ctx.getTurnMgr();
            AuctionWinner aw = am.runAuction(*(getProperty()), tm.getTurnOrder(), tm.getActivePlayerIndex());
            aw.winner.buy(&aw.prop_won, aw.buyAmount, ctx);
            cout << "[" << aw.winner.getUsername() << "] baru saja membeli " << aw.prop_won.getName() << "\n";
            cout << "Uang [" << aw.winner.getUsername() << "] tersisa: " << aw.winner.getBalance() << "\n\n";
            break;
        } else {
            cout << "input tidak valid. Ulangi input.\n";
        }
    }
}

void StreetTile::triggerRentPayment(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    if (player.isShieldActive()) {
        cout << "You have an active shield! No rent paid\n";
        return;
    }

    Property* property = getProperty();
    Player* owner = property->getOwner();
    if (owner == nullptr) {
        return;
    }

    const int rent = property->getRent(ctx);
    cout << "You landed on [" << property->getName() << "] owned by " << owner->getUsername() << ".\n\n";
    property->printStatus(ctx);

    if (player.getBalance() < rent) {
        // GUI mode: show bankruptcy panel with rent info (only for human players)
        if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
            ctx.gameEngine.getPanelManager()->showBankruptcy(ctx, rent);
            ctx.gameEngine.getPanelManager()->getBankruptcyState().pendingRent = rent;
            ctx.gameEngine.getPanelManager()->getBankruptcyState().rentOwner = owner;
            return;
        }
        // Bagian pemanggilan BankruptcyManager secara manual dihapus karena sudah di-handle oleh payRent
    }

    // Menggunakan fungsi payRent dari Player
    player.payRent(property, ctx);
    
    cout << "Money left: <M" << player.getBalance() << ">.\n\n";
}

void RailroadTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::BANK) {
        autoAcquire(player, ctx);
    } else if (property->getStatus() == PropertyStatus::OWNED) {

        if (property->getOwner() != &player) {
            // belum buat sudut pandang player untuk pembayaran dan penerimaan cash
            Property* prop = getProperty();
            cout << "[" << player.getUsername() << "] landed on ["
                 << prop->getName() << "] milik ["
                 << prop->getOwner()->getUsername() << "].\n\n";
            prop->printStatus(ctx); 
            
            // Panggil payRent (otomatis handle deduct, add ke owner, dan cek bangkrut)
            player.payRent(prop, ctx);
            
            cout << "Uang anda tersisa: <M"
                 << player.getBalance()
                 << ">.\n\n";
        }
    } else {
        // kalau Mortgaged lewat aja sih
    }
}

void RailroadTile::autoAcquire(Player& player, TurnContext& ctx){
    Property* prop = getProperty();
    player.addProperty(prop, ctx);
    cout << "[" << prop->getName() << "] is now owned by " << prop->getOwner()->getUsername() << "!\n\n";
}

void UtilityTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    if (property->getStatus() == PropertyStatus::BANK){
        autoAcquire(player, ctx);
        
    } else if (property->getStatus() == PropertyStatus::OWNED){
        if (property->getOwner() != &player){
            Property* prop = getProperty();
            cout << "[" << player.getUsername() << "] landed on ["
                 << prop->getName() << "] milik ["
                 << prop->getOwner()->getUsername() << "].\n\n";
            cout << "Biaya sewa:\nFaktor Pengali:\n";
            prop->printStatus(ctx); 
            
            // Pada kode asli, mekanisme bayar utilitas belum diimplementasi.
            // Sekarang kita langsung delegasikan ke payRent
            player.payRent(prop, ctx);
            
            cout << "Uang anda tersisa: <M" << player.getBalance()
                 << ">.\n\n"; 
        }
    } else {
        // kalau Mortgaged lewat aja sih
    }
}

void UtilityTile::autoAcquire(Player& player, TurnContext& ctx){
    Property* prop = getProperty();
    player.addProperty(prop, ctx);
    cout << "[" << prop->getName() << "] is now owned by " << prop->getOwner()->getUsername() << "!\n\n";
}