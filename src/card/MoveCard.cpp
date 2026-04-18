#include "card/MoveCard.hpp"
#include "player/Player.h"

MoveCard::MoveCard(int stp) : SkillCard("MoveCard: Maju " + std::to_string(stp) + " Petak", 0), steps(stp) {}

void MoveCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] MoveCard: Maju " << steps << " petak.\n";
    
    player.move(steps);
}