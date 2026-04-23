#pragma once

#include <vector>
#include <algorithm>

class Player;
class Board;
class TransactionLogger;

// Forward declarations for card decks
class SkillCard;
class ChanceCard;

class TurnContext {
public:
    TurnContext(Player* player, Board* board, int currentTurn, int maxTurn, 
                std::vector<Player*>& allPlayers);
    
    // Getters
    Player* getCurrentPlayer() const;
    Board* getBoard() const;
    int getCurrentTurn() const;
    int getMaxTurn() const;
    std::vector<Player*>& getAllPlayers();
    
    // Double handling
    void incrementDoubleCount();
    int getDoubleCount() const;
    void resetDoubleCount();
    
    // Skill card usage
    void setSkillUsed(bool used);
    bool isSkillUsed() const;
    
    // Movement tracking
    void setMovedByCard(bool moved);
    bool isMovedByCard() const;
    
    // Transaction logger
    void setLogger(TransactionLogger* logger);
    TransactionLogger* getLogger() const;

    // Fungsi tambahan untuk menambahkan currentturn dan mengubah currentPlayer
    void nextTurn();

private:
    Player* currentPlayer;
    Board* board;
    int currentTurn;
    int maxTurn;
    std::vector<Player*>& allPlayers;
    
    int doubleCount;
    bool skillUsed;
    bool movedByCard;
    TransactionLogger* logger;
};
