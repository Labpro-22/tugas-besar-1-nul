#include "card/MoveCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "tile/Tile.hpp"
#include "board/Board.hpp"
#include "exception/InvalidGameStateException.hpp"

MoveCard::MoveCard(int stp) : SkillCard("MoveCard: Maju " + std::to_string(stp) + " Petak", 0), steps(stp) {}

void MoveCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[Skill] MoveCard: Go forward by " << steps << " tiles\n";
    
    int nextPos = player.move(steps, ctx);
	Tile* baseTile = ctx.board.getTile(nextPos);
	if (baseTile == nullptr) {
		throw InvalidGameStateException("Player moved to an invalid tile index: " + std::to_string(nextPos));
	}
    std::cout << ctx.currentPlayer.getUsername() << " landed in " << baseTile->getName() << "\n";
	baseTile->onLanded(ctx);
}
