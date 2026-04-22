#include "card/ShieldCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"


void ShieldCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] ShieldCard: Anda kebal tagihan selama giliran ini.\n";
    
    player.activateShield();
}
