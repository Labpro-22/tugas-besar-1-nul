#include "core/TurnManager.hpp"
#include "core/Dice.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"

TurnManager::TurnManager(int maxTurn)
    : currentTurn(0), maxTurn(maxTurn > 0 ? maxTurn : -1), activePlayerIndex(-1), hasActedThisTurn(false), turnOrder(), doubleGotten(0) {}

void TurnManager::setTurnOrder(const std::vector<Player*>& players) {
    this->turnOrder.clear();
    this->turnOrder.reserve(players.size());
    for (Player* player : players) {
        if (player != nullptr) {
            this->turnOrder.push_back(player);
        }
    }
    this->activePlayerIndex = this->turnOrder.empty() ? -1 : 0;
}

void TurnManager::addPlayerToOrder(Player* player) {
    if (player == nullptr)
        return;
    this->turnOrder.push_back(player);
    if (this->activePlayerIndex < 0) {
        this->activePlayerIndex = 0;
    }
}

void TurnManager::resetTurns() {
    this->currentTurn = 0;
    this->activePlayerIndex = this->turnOrder.empty() ? -1 : 0;
}

void TurnManager::nextTurn(TurnContext& ctx) {
    if (this->turnOrder.empty())
        return;

    ctx.dice.reset();
    this->activePlayerIndex =
        (this->activePlayerIndex + 1) % this->turnOrder.size();
    this->currentTurn++;
    this->hasActedThisTurn = false;  // Reset flag for new turn
    // ctx.currentPlayer = *this->turnOrder[this->activePlayerIndex];
}

Player* TurnManager::getCurrentPlayer() {
    if (this->turnOrder.empty())
        return nullptr;
    return this->turnOrder[this->activePlayerIndex];
}

int TurnManager::getCurrentTurn() const {
    return this->currentTurn;
}

int TurnManager::getMaxTurn() const {
    return this->maxTurn;
}

int TurnManager::getActivePlayerIndex() const {
    return this->activePlayerIndex;
}

int TurnManager::getDoubleGotten() const {return this->doubleGotten;}

void TurnManager::incrementDoubleGotten(){this->doubleGotten++;}

void TurnManager::resetDoubleGotten() {this->doubleGotten = 0;}

const std::vector<Player*>& TurnManager::getTurnOrder() const { return this->turnOrder; }

void TurnManager::setCurrentTurn(int turn) { this->currentTurn = turn; }

void TurnManager::setActivePlayerIndex(int idx) { this->activePlayerIndex = idx; }

void TurnManager::setMaxTurn(int max) { this->maxTurn = max; }

bool TurnManager::isGameOver() const { 
    if (maxTurn < 0) return false; // nanti bikin kalo smua ud bangkrut
    return this->currentTurn >= this->maxTurn; 
}

std::vector<Player*> TurnManager::determineWinner() {
    std::vector<Player*> winners;
    int maxWealth = -1;

    // coba nnt siapa tau bs pake operator banding player
    for (Player* player : this->turnOrder) {
        if (player == nullptr)
            continue;
        int wealth = player->getWealth();
        if (wealth > maxWealth) {
            winners.clear();
            winners.push_back(player);
            maxWealth = wealth;
        } else if (wealth == maxWealth) {
            winners.push_back(player);
        }
    }

    return winners;
}

bool TurnManager::getHasActedThisTurn() const { return this->hasActedThisTurn; }

void TurnManager::setHasActedThisTurn(bool acted) { this->hasActedThisTurn = acted; }

void TurnManager::markActionTaken() { this->hasActedThisTurn = true; }

void TurnManager::resetActionFlag() { this->hasActedThisTurn = false; }