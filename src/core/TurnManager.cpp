#include "core/TurnManager.hpp"
#include "core/Dice.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"

#include <algorithm>
#include <limits>

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
    int alivePlayers = 0;
    for (Player* player : this->turnOrder) {
        if (player != nullptr && player->getStatus() != PlayerStatus::BANKRUPT) {
            ++alivePlayers;
        }
    }

    // Bankruptcy mode (invalid max turn in config): game ends when one player remains.
    if (this->maxTurn < 1) {
        return alivePlayers <= 1;
    }

    // Even in max-turn mode, if bankruptcy finishes the game earlier, stop immediately.
    if (alivePlayers <= 1) {
        return true;
    }

    // Max turn is defined per-player round, so all players must complete maxTurn turns.
    const int playersCount = static_cast<int>(this->turnOrder.size());
    if (playersCount <= 0) {
        return true;
    }

    return this->currentTurn >= (this->maxTurn * playersCount);
}

std::vector<Player*> TurnManager::determineWinner() {
    std::vector<Player*> eligible;
    for (Player* player : this->turnOrder) {
        if (player != nullptr && player->getStatus() != PlayerStatus::BANKRUPT) {
            eligible.push_back(player);
        }
    }

    // Bankruptcy finish: only one surviving player.
    if (eligible.size() <= 1) {
        return eligible;
    }

    auto filterByBest = [](std::vector<Player*> candidates,
                           auto metric) {
        if (candidates.empty()) {
            return candidates;
        }

        int best = std::numeric_limits<int>::min();
        for (Player* p : candidates) {
            best = std::max(best, metric(p));
        }

        std::vector<Player*> filtered;
        for (Player* p : candidates) {
            if (metric(p) == best) {
                filtered.push_back(p);
            }
        }
        return filtered;
    };

    // Max-turn tie-breaker: balance -> properties -> cards in hand.
    eligible = filterByBest(eligible, [](Player* p) { return p->getBalance(); });
    if (eligible.size() == 1) {
        return eligible;
    }

    eligible = filterByBest(eligible, [](Player* p) { return p->getPropertiesAmount(); });
    if (eligible.size() == 1) {
        return eligible;
    }

    eligible = filterByBest(eligible, [](Player* p) { return p->getHandsAmount(); });
    return eligible;
}

bool TurnManager::getHasActedThisTurn() const { return this->hasActedThisTurn; }

void TurnManager::setHasActedThisTurn(bool acted) { this->hasActedThisTurn = acted; }

void TurnManager::markActionTaken() { this->hasActedThisTurn = true; }

void TurnManager::resetActionFlag() { this->hasActedThisTurn = false; }