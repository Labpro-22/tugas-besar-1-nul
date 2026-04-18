#include "card/ShieldCard.hpp"
#include "player/Player.h"

ShieldCard::ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

void ShieldCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] ShieldCard: Anda kebal tagihan selama giliran ini.\n";
    
    player.activateShield();
}


