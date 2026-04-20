#include "card/ShieldCard.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"

ShieldCard::ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

void ShieldCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] ShieldCard: Anda kebal tagihan selama giliran ini.\n";
    
    player.activateShield();
}


