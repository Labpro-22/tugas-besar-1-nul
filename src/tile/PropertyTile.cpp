#include "tile/PropertyTile.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "core/BankruptcyManager.hpp"
#include "exception/BankruptcyException.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"
#include "gui/GuiPanelManager.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include "core/TurnManager.hpp"
#include "core/AuctionManager.hpp"
#include <limits>

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
    cout << "\n";

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
        cout << "Do you want to buy " << getName()
             << "? (Price: " << getProperty()->getBuyPrice() << ") [Y/n]\n";
        cout << "> ";
        cin >> ans;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear newline from buffer
        if (ans == "Y" || ans == "y"){
            player.buy(getProperty(), ctx);
            cout << "\n";
            cout << "[" << player.getUsername() << "] just bought " << getName() << "\n";
            cout << "[" << player.getUsername() << "]'s remaining money: " << player.getBalance() << "\n\n";
            break;
        } else if (ans == "N" || ans == "n"){
            AuctionManager am;
            TurnManager tm = ctx.getTurnMgr();
            AuctionWinner aw = am.runAuction(*(getProperty()), tm.getTurnOrder(), tm.getActivePlayerIndex());
            aw.winner.buy(&aw.prop_won, aw.buyAmount, ctx);
            cout << "\n";
            cout << "[" << aw.winner.getUsername() << "] just won the auction for " << aw.prop_won.getName() << "\n";
            cout << "[" << aw.winner.getUsername() << "]'s remaining money: " << aw.winner.getBalance() << "\n\n";
            break;
        } else {
            cout << "Invalid input! Please retry\n\n";
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
        const bool canContinue = triggerBankruptcy(ctx, rent);
        if (!canContinue) {
            return;
        }
    }

    player.deductCash(rent);
    owner->addCash(rent);
    cout << "Rent paid: M" << rent << "\n";
    cout << "Money left: <M" << player.getBalance() << ">.\n\n";
}

bool StreetTile::triggerBankruptcy(TurnContext& ctx, int debtAmount) {
    Player& player = ctx.currentPlayer;

    // GUI mode: show bankruptcy panel (only for human players)
    if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
        ctx.gameEngine.getPanelManager()->showBankruptcy(ctx, debtAmount);
        return false; // Defer to panel; caller should not continue immediately
    }

    while (player.getBalance() < debtAmount) {
        auto options = BankruptcyManager::getAvailableLiquidationOptions(ctx);
        if (options.empty()) {
            BankruptcyManager::declareBankrupt(ctx);
            cout << player.getUsername() << " cannot cover rent and is bankrupt\n";
            return false;
        }

        cout << "=== Panel Likuidasi ===\n";
        cout << "Uang kamu saat ini: M" << player.getBalance()
             << "  |  Kewajiban: M" << debtAmount << "\n\n";

        vector<LiquidationOption> sellOptions;
        vector<LiquidationOption> mortgageOptions;
        for (const auto& option : options) {
            if (option.property == nullptr || option.property->getStatus() != PropertyStatus::OWNED) {
                continue;
            }

            if (option.type == LiquidationOption::SELL) {
                sellOptions.push_back(option);
            } else {
                mortgageOptions.push_back(option);
            }
        }

        vector<LiquidationOption> numberedOptions;
        int menuIndex = 1;

        cout << "[Jual ke Bank]\n";
        if (sellOptions.empty()) {
            cout << "- Tidak ada properti yang bisa dijual\n";
        } else {
            for (const auto& option : sellOptions) {
                Property* prop = option.property;
                string typeLabel = "LAIN";
                string extraLabel;

                if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
                    typeLabel = street->getColorGroup();
                    if (street->hasHotel()) {
                        const int housePart = 4 * street->getHousePrice();
                        const int hotelPart = street->getHotelPrice();
                        extraLabel = " (termasuk hotel: M" + to_string(housePart + hotelPart) + ")";
                    } else if (street->getBuildingCount() > 0) {
                        const int buildingPart = street->getBuildingCount() * street->getHousePrice();
                        extraLabel = " (termasuk " + to_string(street->getBuildingCount()) +
                                     " rumah: M" + to_string(buildingPart / 2) + ")";
                    }
                } else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
                    typeLabel = "STATION";
                } else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
                    typeLabel = "UTILITY";
                }

                cout << menuIndex << ". " << prop->getName() << " (" << prop->getCode() << ")"
                     << "  [" << typeLabel << "]"
                     << "  Harga Jual: M" << option.cashValue
                     << extraLabel << "\n";
                numberedOptions.push_back(option);
                ++menuIndex;
            }
        }

        cout << "\n[Gadaikan]\n";
        if (mortgageOptions.empty()) {
            cout << "- Tidak ada properti yang bisa digadaikan\n";
        } else {
            for (const auto& option : mortgageOptions) {
                Property* prop = option.property;
                string typeLabel = "LAIN";
                if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
                    typeLabel = street->getColorGroup();
                } else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
                    typeLabel = "STATION";
                } else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
                    typeLabel = "UTILITY";
                }

                cout << menuIndex << ". " << prop->getName() << " (" << prop->getCode() << ")"
                     << "  [" << typeLabel << "]"
                     << "  Nilai Gadai: M" << option.cashValue << "\n";
                numberedOptions.push_back(option);
                ++menuIndex;
            }
        }

        if (numberedOptions.empty()) {
            BankruptcyManager::declareBankrupt(ctx);
            cout << player.getUsername() << " cannot cover rent and is bankrupt\n";
            return false;
        }

        int selected = 0;
        cout << "\nPilih aksi likuidasi (nomor): ";
        while (!(cin >> selected) || selected < 1 || selected > static_cast<int>(numberedOptions.size())) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid, masukkan nomor yang benar: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        const LiquidationOption choice = numberedOptions[static_cast<size_t>(selected - 1)];
        if (choice.property == nullptr || choice.property->getStatus() != PropertyStatus::OWNED) {
            cout << "Properti tidak valid untuk diproses.\n\n";
            continue;
        }

        try {
            if (choice.type == LiquidationOption::MORTGAGE) {
                player.mortgage(choice.property);
                cout << "[LIKUIDASI] Berhasil menggadaikan " << choice.property->getName()
                     << " untuk M" << choice.cashValue << "\n\n";
            } else {
                const int saleValue = choice.property->sellToBank();
                player.addCash(saleValue);
                player.sell(*choice.property);
                cout << "[LIKUIDASI] Berhasil menjual " << choice.property->getName()
                     << " ke bank untuk M" << saleValue << "\n\n";
            }
        } catch (const std::exception& ex) {
            cout << "[LIKUIDASI] Gagal: " << ex.what() << "\n\n";
        }
    }

    return true;
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
            player.deductCash(prop->getRent(ctx)); // nanti aktivasi mode bankruptcy
            (property->getOwner())->addCash(prop->getRent(ctx));
            cout << "Uang anda tersisa: <M"
                 << player.getBalance() - prop->getRent(ctx)
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