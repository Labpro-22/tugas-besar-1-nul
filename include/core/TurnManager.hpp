#pragma once

#include <vector>

class Player;
class TurnContext;

class TurnManager {
  private:
    int currentTurn;
    int maxTurn;
    int activePlayerIndex;

    std::vector<Player*> turnOrder;

  public:
    explicit TurnManager(int maxTurn = -1);
    ~TurnManager() = default;

    void setTurnOrder(const std::vector<Player*>& players);
    void addPlayerToOrder(Player* player);
    void resetTurns();

    void nextTurn(TurnContext& ctx);
    Player* getCurrentPlayer();

    int getCurrentTurn() const;
    int getMaxTurn() const;
    int getActivePlayerIndex() const;
    const std::vector<Player*>& getTurnOrder() const;

    bool isGameOver() const;
    std::vector<Player*> determineWinner();
};