#include "card/MoveCard.hpp"
#include "core/TurnContext.hpp"
#include "tile/Tile.hpp"
#include "board/Board.hpp"
#include "player/Player.hpp"
#include "exception/InvalidGameStateException.hpp"

MoveCard::MoveCard(int stp) : SkillCard("|Move Card|: Move forward by " + std::to_string(stp) + " tiles", 0), steps(stp) {
    if (stp <= 0) {
        throw InvalidGameStateException("Move Card steps must be positive, got: " + std::to_string(stp));
    }
}

void MoveCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[SKILL] " << this->getDescription() << "\n";
    
    int nextPos = player.move(steps, ctx);
	Tile* baseTile = ctx.board.getTile(nextPos);
	if (baseTile == nullptr) {
		throw InvalidGameStateException("Player moved to an invalid tile index: " + std::to_string(nextPos));
	}
    // std::cout << ctx.currentPlayer.getUsername() << " landed in " << baseTile->getName() << "\n";
	baseTile->onLanded(ctx);
}
