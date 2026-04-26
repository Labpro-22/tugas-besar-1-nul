#include "tile/ActionTile.hpp"
#include "board/Board.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "player/Player.hpp"
#include "tile/PropertyTile.hpp"
#include "card/ChanceCard.hpp"
#include "card/CommunityChestCard.hpp"
#include "gui/GuiPanelManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

ActionTile::ActionTile(int idx, string cd, string nm) : Tile(idx, cd, nm) {}

void ActionTile::onLanded(TurnContext& ctx) {}

GoTile::GoTile(int idx, string cd, string nm) : ActionTile(idx, cd, nm) {}

JailTile::JailTile(int idx, string cd, string nm) : ActionTile(idx, cd, nm) {}

void JailTile::onLanded(TurnContext& ctx) {}

void JailTile::addInmate(Player& player) {
    inmates.push_back(&player);
    // pindahkan player ke penjara
    cout << "[" << player.getUsername() << "] got in jail.\n";
}

void JailTile::removeInmate(Player& player) {
    auto it = find(inmates.begin(), inmates.end(), &player);
    if (it != inmates.end()) {
        inmates.erase(it);
    }
    cout << "[" << player.getUsername() << "] got out of jail.\n";
}

bool JailTile::isInmate(Player& player) {
    return find(inmates.begin(), inmates.end(), &player) != inmates.end();
}

FreeParkingTile::FreeParkingTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

GoToJailTile::GoToJailTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void GoToJailTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::vector<Tile*>& allTiles = ctx.board.getAllTiles();
    JailTile* targetJail = nullptr;
    for (Tile* tile : allTiles) {
        targetJail = dynamic_cast<JailTile*>(tile);
        if (targetJail != nullptr) {
            break;
        }
    }

    if (targetJail != nullptr) {
        targetJail->addInmate(player);
    } else {
        // in case tidak ada petak penjara, bikin exception nanti
        std::cout << "Sistem Error: Tidak ada penjara di kota ini!\n";
    }
}

CardTile::CardTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm), cardType(CardTileType::CHANCE) {
    // Determine card type based on code
    if (cd == "DNU") {
        cardType = CardTileType::COMMUNITY_CHEST;
    } else if (cd == "KSP") {
        cardType = CardTileType::CHANCE;
    }
}

CardTile::CardTile(int idx, string cd, string nm, CardTileType type)
    : ActionTile(idx, cd, nm), cardType(type) {}

void CardTile::setCardType(CardTileType type) {
    cardType = type;
}

CardTileType CardTile::getCardType() const {
    return cardType;
}

void CardTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    
    if (cardType == CardTileType::CHANCE) {
        std::cout << "[" << player.getUsername() << "] landed on [Chance Tile].\n";
        ChanceCard* card = ctx.drawChanceCard();
        if (card != nullptr) {
            if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
                ctx.gameEngine.getPanelManager()->showCardReveal(card, nullptr, player, ctx);
                return;
            }
            
            card->execute(&player, ctx);
            ctx.returnChanceCard(card);
        } else {
            std::cout << "[INFO] No Chance cards available in the deck.\n";
        }
    } else {
        std::cout << "[" << player.getUsername() << "] landed on [Community Chest Tile].\n";
        CommunityChestCard* card = ctx.drawCommunityChestCard();
        if (card != nullptr) {
            if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
                ctx.gameEngine.getPanelManager()->showCardReveal(nullptr, card, player, ctx);
                return;
            }
            
            card->execute(&player, ctx);
            ctx.returnCommunityChestCard(card);
        } else {
            std::cout << "No Community Chest cards available in the deck.\n";
        }
    }
}

FestivalTile::FestivalTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void FestivalTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;

    // GUI mode: show festival panel (only for human players)
    if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
        ctx.gameEngine.getPanelManager()->showFestival(player, ctx);
        return;
    }

    // Bot players skip festival selection
    if (player.isBot()) {
        return;
    }

    cout << "[" << player.getUsername() << "] landed on [Festival Tile].\n\n";

    cout << "Choose property to increase rent: \n";
    bool hasProp = false;
    std::vector<Tile*>& allTiles = ctx.board.getAllTiles();
    PropertyTile* proptile = nullptr;
    for (Tile* tile : allTiles) {
        proptile = dynamic_cast<PropertyTile*>(tile);
        if (proptile != nullptr) {
            if (proptile->getProperty()->getOwner() == &ctx.currentPlayer) {
                proptile->getProperty()->printStatus(ctx);
                hasProp = true;
            }
        }
    }
    if (!hasProp){
        cout << "[" << ctx.currentPlayer.getUsername() << "] doesn't have any property to increase the rent of.\n";
        return;
    }
    std::string inp;
    while (true) {
        cout << "Enter property code chosen to be applied Festival: ";
        cin >> inp;
        Tile* selectedTile = ctx.board.getTileByCode(inp);

        if (selectedTile != nullptr) {
            proptile = dynamic_cast<PropertyTile*>(selectedTile);

            // Validasi Ganda
            if (proptile != nullptr &&
                proptile->getProperty()->getOwner() == &player) {
                break; // Input valid! Keluar dari loop.
            } else {
                cout << "[ERROR] Properti tidak valid atau bukan milik "
                        "Anda!\n\n"; // nanti ganti jadi exception
            }
        } else {
            cout << "[ERROR] Kode petak tidak ditemukan!\n\n"; // nanti ganti
                                                               // jadi exception
        }
    }
    applyFestival(player, *(proptile->getProperty()), ctx);
}

void FestivalTile::applyFestival(Player& player, Property& prop, TurnContext& ctx) {
    cout << "Applying Festival Mode to property: " << prop.getName() << "\n";
    prop.applyFestival();
    cout << "Now the rent for " << prop.getName() << " is M" << prop.getRent(ctx) << "\n";
}

TaxTile::TaxTile(int idx, string cd, string nm, TaxType type)
    : ActionTile(idx, cd, nm), taxType(type) {}

void TaxTile::onLanded(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;

    cout << "[" << player.getUsername() << "] landed on [Tax Tile].\n\n";
    if (taxType == TaxType::PPH) { // is PPH
        // GUI mode: show tax panel (only for human players)
        if (!player.isBot() && ctx.gameEngine.getPanelManager()) {
            ctx.gameEngine.getPanelManager()->showTax(
                player,
                ctx.gameEngine.getTaxPphFlat(),
                static_cast<float>(ctx.gameEngine.getTaxPphPercent()));
            return;
        }
        applyPPH(player, ctx);
    } else if (taxType == TaxType::PBM) { // is PBM
        applyPBM(player, ctx);
    }
}

void TaxTile::applyPPH(Player& player, TurnContext& ctx) {
    int taxFlat = ctx.gameEngine.getTaxPphFlat();
    int taxPercent = ctx.gameEngine.getTaxPphPercent();

    // Bot players auto-pay flat tax
    if (player.isBot()) {
        player.deductCash(taxFlat);
        cout << "[" << player.getUsername() << "] paid tax of M" << taxFlat << "\n";
        return;
    }

    cout << "You have 2 options to pay your tax:\n";
    cout << "1. Pay flat tax of M" << taxFlat << "\n";
    cout << "2. Pay percentage tax of " << taxPercent << "\% of your current balance\n";


    int inp;
    while (true){
        cout << "> ";
        cin >> inp;
        if (inp == 1 || inp == 2) {
            break;
        }
        cout << "Invalid input! Please retry\n\n";
    }
    if (inp == 1) {
        player.deductCash(taxFlat);
        cout << "[" << player.getUsername() << "] paid tax of M" << taxFlat << "\n";
    } else{
        int percentTaxAmount = static_cast<int>(std::ceil(
            (static_cast<double>(taxPercent) / 100.0) * static_cast<double>(player.getBalance())));
        player.deductCash(percentTaxAmount);
        cout << "[" << player.getUsername() << "] paid tax of M" << percentTaxAmount << "\n\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void TaxTile::applyPBM(Player& player, TurnContext& ctx) {
    int taxFlat = ctx.gameEngine.getTaxPbmFlat();
    player.deductCash(taxFlat);
    cout << "[" << player.getUsername() << "] paid tax of M" << taxFlat << "\n\n";
}
