#include "core/TurnManager.hpp"

#include "player/Player.h"

TurnManager::TurnManager(int maxTurn)
    : currentTurn(0), maxTurn(maxTurn > 0 ? maxTurn : -1), activePlayerIndex(-1), turnOrder() {}

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
    if (player == nullptr) return;
    this->turnOrder.push_back(player);
    if (this->activePlayerIndex < 0) {
        this->activePlayerIndex = 0;
    }
}

void TurnManager::resetTurns() {
    this->currentTurn = 0;
    this->activePlayerIndex = this->turnOrder.empty() ? -1 : 0;
}

void TurnManager::nextTurn() {
    if (this->turnOrder.empty()) return;

    if (this->activePlayerIndex < 0) {
        this->activePlayerIndex = 0;
        return;
    }

    this->activePlayerIndex = (this->activePlayerIndex + 1) % this->turnOrder.size();
    this->currentTurn++;
}

Player* TurnManager::getCurrentPlayer() {
    if (this->turnOrder.empty()) return nullptr;

    if (this->activePlayerIndex < 0) {
        this->activePlayerIndex = 0;
    }

    if (this->activePlayerIndex >= static_cast<int>(this->turnOrder.size())) {
        this->activePlayerIndex %= this->turnOrder.size();
    }

    return this->turnOrder[this->activePlayerIndex];
}

int TurnManager::getCurrentTurn() const { return this->currentTurn; }

int TurnManager::getMaxTurn() const { return this->maxTurn; }

int TurnManager::getActivePlayerIndex() const { return this->activePlayerIndex; }

const std::vector<Player*>& TurnManager::getTurnOrder() const { return this->turnOrder; }

bool TurnManager::isGameOver() const { 
    if (maxTurn < 0) return false; // nanti bikin kalo smua ud bangkrut
    return this->currentTurn >= this->maxTurn; 
}

std::vector<Player*> TurnManager::determineWinner() {
    std::vector<Player*> winners;
    int maxWealth = -1;

    // coba nnt siapa tau bs pake operator banding player
    for (Player* player : this->turnOrder) {
        if (player == nullptr) continue;
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