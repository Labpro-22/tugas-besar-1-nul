#include "core/TurnContext.hpp"
#include "board/Board.hpp"
#include "core/Dice.hpp"
#include "core/GameEngine.hpp"
#include "player/Player.hpp"
#include "tile/Tile.hpp"

Tile& TurnContext::getTile() const {
    return *board.getTile(currentPlayer.getPosition());
}

int TurnContext::getBoardSize() const {
    return this->board.getSize();
}

TurnManager& TurnContext::getTurnMgr() const {
    return gameEngine.getTurnManager();
}

const std::vector<Player*>& TurnContext::getAllPlayers() const {
    return this->gameEngine.getPlayers();
}