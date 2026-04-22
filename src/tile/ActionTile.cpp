#include "tile/ActionTile.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

ActionTile::ActionTile(int idx, string cd, string nm, string cat)
    : Tile(idx, cd, nm, cat) {}

void ActionTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik ActionTile!\n";
};

GoTile::GoTile(int idx, string cd, string nm, string cat)
    : ActionTile(idx, cd, nm, cat) {}

void GoTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik GoTile!\n";
};

void GoTile::paySalary(Player* player, int amount){
    cout << "paySalary milik GoTile\n";
    player->addCash(amount);
};

JailTile::JailTile(int idx, string cd, string nm, string cat)
    : ActionTile(idx, cd, nm, cat) {}

void JailTile::onLanded(Player* player, TurnContext& ctx){
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

FreeParkingTile::FreeParkingTile(int idx, string cd, string nm, string cat)
    : ActionTile(idx, cd, nm, cat) {}

void FreeParkingTile::onLanded(Player* player, TurnContext& ctx){
    cout << "Actually onLanded in FreeParkingTile does nothing\n";
};

GoToJailTile::GoToJailTile(int idx, string cd, string nm, string cat)
    : ActionTile(idx, cd, nm, cat) {}

void GoToJailTile::onLanded(Player* player, TurnContext& ctx){
    cout << "what crime did u commit?\n";
    //addInmate(player); 
};

CardTile::CardTile(int idx, string cd, string nm, string cat, bool chance)
    : ActionTile(idx, cd, nm, cat), isChance(chance) {}

void CardTile::onLanded(Player* player, TurnContext& ctx){
    if (isChance) {
        cout << "Mengambil kartu Kesempatan!\n";
    } else {
        cout << "Mengambil kartu Dana Umum!\n";
    }
};

FestivalTile::FestivalTile(int idx, string cd, string nm, string cat)
    : ActionTile(idx, cd, nm, cat) {}

void FestivalTile::onLanded(Player* player, TurnContext& ctx){
    cout << "u are in luck, if u have any property\n";
    // should do applyFestival after receiving which prop to apply
};

void FestivalTile::applyFestival(Player* player, StreetProperty* prop){
    cout << "we be rich bro\n";
}

TaxTile::TaxTile(int idx, string cd, string nm, string cat, TaxType type)
    : ActionTile(idx, cd, nm, cat), taxType(type) {}

void TaxTile::onLanded(Player* player, TurnContext& ctx){
    cout << "haha got taxed\n";
    if (taxType == TaxType::PPH){ //is PPH
        applyPPH(player);
    }
    else if (taxType == TaxType::PBM){ //is PBM
        applyPBM(player);
    }
};

void TaxTile::applyPPH(Player* player){
    cout << "PPh applied, u now poor\n";
}

void TaxTile::applyPBM(Player* player){
    cout << "PBM applied, u now poor\n";
}
