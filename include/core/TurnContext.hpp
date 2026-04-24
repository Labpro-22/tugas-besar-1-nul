#pragma once

#include <vector>
#include <algorithm>

class Player;
class Board;
class TransactionLogger;
class Tile;
class Dice;

// Forward declarations for card decks
class SkillCard;
class ChanceCard;
class CommunityChestCard;
class GameEngine;
class TurnManager;


class TurnContext {
public:
    Player& currentPlayer;
    Dice& dice;
    Board& board;
    GameEngine& gameEngine;

    TurnContext(Player& player, Dice& dice, Board& board, GameEngine& gameEngine)
        : currentPlayer(player), dice(dice), board(board), gameEngine(gameEngine) {}

    
    Tile& getTile() const;
    int getBoardSize() const;

    TurnManager& getTurnMgr() const;
    const std::vector<Player*>& getAllPlayers() const;

    // Card deck methods
    ChanceCard* drawChanceCard();
    CommunityChestCard* drawCommunityChestCard();
    void returnChanceCard(ChanceCard* card);
    void returnCommunityChestCard(CommunityChestCard* card);

    // current/max turn num
    // skillused, movedbycard, doublecount <- turncontext
};