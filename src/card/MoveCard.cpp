#include "MoveCard.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"

MoveCard::MoveCard(int stp) : SkillCard("MoveCard: Maju " + std::to_string(stp) + " Petak", 0), steps(stp) {}

void MoveCard::apply(TurnContext& ctx) {
    Player& player = *ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] MoveCard: Maju " << steps << " petak.\n";
    
    player.move(steps);
}