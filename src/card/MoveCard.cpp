#include "card/MoveCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "exception/InvalidGameStateException.hpp"

MoveCard::MoveCard(int stp) : SkillCard("MoveCard: Maju " + std::to_string(stp) + " Petak", 0), steps(stp) {
    if (stp <= 0) {
        throw InvalidGameStateException("MoveCard steps must be positive, got: " + std::to_string(stp));
    }
}

void MoveCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] MoveCard: Maju " << steps << " petak.\n";
    
    player.move(steps, ctx);
}
