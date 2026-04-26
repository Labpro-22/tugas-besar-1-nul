#include "card/TeleportCard.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "exception/InvalidGameStateException.hpp"

TeleportCard::TeleportCard(int pos) : SkillCard("|Teleport Card|: Teleport to " + std::to_string(pos), 0), position(pos) {
    if (pos < 0) {
        throw InvalidGameStateException("Teleport Card position cannot be negative, got: " + std::to_string(pos));
    }
}

void TeleportCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    int boardSize = ctx.getBoardSize();

    std::cout << "[SKILL] " << this->getDescription() << "\n";
    
    if (position >= boardSize) {
        throw InvalidGameStateException("Teleport Card position " + std::to_string(position) + 
                                       " exceeds board size " + std::to_string(boardSize));
    }
    
    std::cout << "[SKILL] Teleport: Teleporting to tile " << position << ".\n";
    
    player.moveForwardTo(position, ctx);
}
