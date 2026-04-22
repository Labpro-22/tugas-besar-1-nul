#include "tile/ActionTile.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "board/Board.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

ActionTile::ActionTile(int idx, string cd, string nm)
    : Tile(idx, cd, nm) {}

void ActionTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "onLanded milik ActionTile!\n";
};

GoTile::GoTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void GoTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "onLanded milik GoTile!\n";
};

void GoTile::paySalary(Player* player, int amount){
    cout << "paySalary milik GoTile\n";
    player->addCash(amount);
};

JailTile::JailTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void JailTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "You are at jail brader\n";
};

void JailTile::addInmate(Player* player){
    cout << "You are jailed bro\n";
    inmates.push_back(player);
}

void JailTile::removeInmate(Player* player){
    cout << "Dont come here again bro\n";
    auto it = find(inmates.begin(), inmates.end(), player);
    if (it != inmates.end()) {
        inmates.erase(it);
    }
}

bool JailTile::isInmate(Player* player){
    cout << "are u jailed?\n";
    return find(inmates.begin(), inmates.end(), player) != inmates.end();
}

FreeParkingTile::FreeParkingTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void FreeParkingTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "Actually onLanded in FreeParkingTile does nothing\n";
};

GoToJailTile::GoToJailTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void GoToJailTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "what crime did u commit?\n";
    //addInmate(player); 
};

CardTile::CardTile(int idx, string cd, string nm, bool chance)
    : ActionTile(idx, cd, nm), isChance(chance) {}

void CardTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    if (isChance) {
        cout << "Mengambil kartu Kesempatan!\n";
    } else {
        cout << "Mengambil kartu Dana Umum!\n";
    }
};

FestivalTile::FestivalTile(int idx, string cd, string nm)
    : ActionTile(idx, cd, nm) {}

void FestivalTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "u are in luck, if u have any property\n";
    // should do applyFestival after receiving which prop to apply
};

void FestivalTile::applyFestival(Player* player, StreetProperty* prop){
    cout << "we be rich bro\n";
}

TaxTile::TaxTile(int idx, string cd, string nm, TaxType type)
    : ActionTile(idx, cd, nm), taxType(type) {}

void TaxTile::onLanded(TurnContext& ctx){
    Player& player = ctx.currentPlayer;
    cout << "haha got taxed\n";
    if (taxType == TaxType::PPH){ //is PPH
        applyPPH(player);
    }
    else if (taxType == TaxType::PBM){ //is PBM
        applyPBM(player);
    }
};

void TaxTile::applyPPH(Player& player){
    cout << "PPh applied, u now poor\n";
}

void TaxTile::applyPBM(Player& player){
    cout << "PBM applied, u now poor\n";
}
