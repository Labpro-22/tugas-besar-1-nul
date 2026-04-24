#pragma once

#include <vector>

class Player;
class TurnContext;

class TurnManager {
  private:
    int currentTurn;
    int maxTurn;
    int activePlayerIndex;
    bool hasActedThisTurn;  // Track if current player has taken any action

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

    void setCurrentTurn(int turn);
    void setActivePlayerIndex(int idx);
    void setMaxTurn(int max);

    // Action tracking for save restriction
    bool getHasActedThisTurn() const;
    void setHasActedThisTurn(bool acted);
    void markActionTaken();  // Call this when player takes any action
    void resetActionFlag();  // Call this when turn starts

    bool isGameOver() const;
    std::vector<Player*> determineWinner();
};