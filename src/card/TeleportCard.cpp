#include "card/TeleportCard.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"

TeleportCard::TeleportCard(int pos) : SkillCard("TeleportCard: Pindah ke petak manapun", 0), position(pos) {}

void TeleportCard::apply(TurnContext& ctx) {
    Player& player = *ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] TeleportCard: Memulai proses teleportasi.\n";
    
    player.moveForwardTo(position);
}