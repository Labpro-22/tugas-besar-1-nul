#include "TeleportCard.hpp"
#include "player/Player.h"

TeleportCard::TeleportCard(int pos) : SkillCard("TeleportCard: Pindah ke petak manapun", 0), position(pos) {}

void TeleportCard::apply(Player& player)  {
    std::cout << "[MENGGUNAKAN] TeleportCard: Memulai proses teleportasi.\n";
    
    player.moveForwardTo(position);
}