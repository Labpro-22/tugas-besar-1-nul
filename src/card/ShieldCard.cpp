#include "card/ShieldCard.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"

void ShieldCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] ShieldCard: Anda kebal tagihan selama giliran ini.\n";
    
    player.activateShield();
}
