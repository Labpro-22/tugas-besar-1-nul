#include "tile/ActionTile.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "board/Board.hpp"

void ActionTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik ActionTile!\n";
};

void GoTile::onLanded(Player* player, TurnContext& ctx){
    cout << "onLanded milik GoTile!\n";
};

void GoTile::paySalary(Player* player){
    cout << "paySalary milik GoTile\n";
    //set salary player += yang di spek pokoknya
};

void JailTile::onLanded(Player* player, TurnContext& ctx){
    cout << "You are at jail brader\n";
};

void JailTile::addInmate(Player* player){
    cout << "You are jailed bro\n";
}

void JailTile::removeInmate(Player* player){
    cout << "Dont come here again bro\n";
}

bool JailTile::isInmate(Player* player){
    cout << "are u jailed?\n";
    return false;
}

void FreeParkingTile::onLanded(Player* player, TurnContext& ctx){
    cout << "Actually onLanded in FreeParkingTile does nothing\n";
};

void GoToJailTile::onLanded(Player* player, TurnContext& ctx){
    cout << "what crime did u commit?\n";
    //addInmate(player); 
};

void CardTile::onLanded(Player* player, TurnContext& ctx){
    cout << "lets see what card u get\n";
};

void FestivalTile::onLanded(Player* player, TurnContext& ctx){
    cout << "u are in luck, if u have any property\n";
    // should do applyFestival after receiving which prop to apply
};

void FestivalTile::applyFestival(Player* player, StreetProperty* prop){
    cout << "we be rich bro\n";
}

void TaxTile::onLanded(Player* player, TurnContext& ctx){
    cout << "haha got taxed\n";
    if (taxType == TaxType::PPH){ //is PPH
        applyPPH(player);
    }
    else if (taxType == TaxType::PPN){ //is PPN
        applyPPN(player);
    }
};

void TaxTile::applyPPH(Player* player){
    cout << "PPh applied, u now poor\n";
}

void TaxTile::applyPPN(Player* player){
    cout << "PPn applied, u now poor\n";
}