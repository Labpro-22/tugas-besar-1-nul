#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "board/Board.hpp"
#include "core/GameEngine.hpp"
#include "tile/Tile.hpp"
#include "core/Dice.hpp"

Tile& TurnContext::getTile() const {
    return *board.getTile(currentPlayer.getPosition());
}

int TurnContext::getBoardSize() const {
    return this->board.getSize();
}

const std::vector<Player*>& TurnContext::getAllPlayers() const {
    return this->gameEngine.getPlayers();
}