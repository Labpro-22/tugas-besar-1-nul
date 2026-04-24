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

TurnManager& TurnContext::getTurnMgr() const {
    return gameEngine.getTurnManager();
}

const std::vector<Player*>& TurnContext::getAllPlayers() const {
    return this->gameEngine.getPlayers();
}

ChanceCard* TurnContext::drawChanceCard() {
    return gameEngine.drawChanceCard();
}

CommunityChestCard* TurnContext::drawCommunityChestCard() {
    return gameEngine.drawCommunityChestCard();
}

void TurnContext::returnChanceCard(ChanceCard* card) {
    gameEngine.returnChanceCard(card);
}

void TurnContext::returnCommunityChestCard(CommunityChestCard* card) {
    gameEngine.returnCommunityChestCard(card);
}