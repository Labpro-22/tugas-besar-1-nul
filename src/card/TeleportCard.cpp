#include "card/TeleportCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "exception/InvalidGameStateException.hpp"

TeleportCard::TeleportCard(int pos) : SkillCard("TeleportCard: Pindah ke petak manapun", 0), position(pos) {
    if (pos < 0) {
        throw InvalidGameStateException("TeleportCard position cannot be negative, got: " + std::to_string(pos));
    }
}

void TeleportCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    int boardSize = ctx.getBoardSize();
    
    if (position >= boardSize) {
        throw InvalidGameStateException("TeleportCard position " + std::to_string(position) + 
                                       " exceeds board size " + std::to_string(boardSize));
    }
    
    std::cout << "[MENGGUNAKAN] TeleportCard: Memulai proses teleportasi ke petak " << position << ".\n";
    
    player.moveForwardTo(position, ctx);
}
