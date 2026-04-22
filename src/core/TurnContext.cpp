#include "core/TurnContext.hpp"
#include "player/Player.h"
#include "board/Board.hpp"

TurnContext::TurnContext(Player* player, Board* board, int currentTurn, int maxTurn,
                         std::vector<Player*>& allPlayers)
    : currentPlayer(player),
      board(board),
      currentTurn(currentTurn),
      maxTurn(maxTurn),
      allPlayers(allPlayers),
      doubleCount(0),
      skillUsed(false),
      movedByCard(false),
      logger(nullptr) {}

Player* TurnContext::getCurrentPlayer() const {
    return currentPlayer;
}

Board* TurnContext::getBoard() const {
    return board;
}

int TurnContext::getCurrentTurn() const {
    return currentTurn;
}

int TurnContext::getMaxTurn() const {
    return maxTurn;
}

std::vector<Player*>& TurnContext::getAllPlayers() {
    return allPlayers;
}

void TurnContext::incrementDoubleCount() {
    doubleCount++;
}

int TurnContext::getDoubleCount() const {
    return doubleCount;
}

void TurnContext::resetDoubleCount() {
    doubleCount = 0;
}

void TurnContext::setSkillUsed(bool used) {
    skillUsed = used;
}

bool TurnContext::isSkillUsed() const {
    return skillUsed;
}

void TurnContext::setMovedByCard(bool moved) {
    movedByCard = moved;
}

bool TurnContext::isMovedByCard() const {
    return movedByCard;
}

void TurnContext::setLogger(TransactionLogger* log) {
    logger = log;
}

TransactionLogger* TurnContext::getLogger() const {
    return logger;
}
