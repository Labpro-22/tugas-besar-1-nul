#pragma once

#include <vector>
#include <algorithm>

class Player;
class Board;
class TransactionLogger;
class Tile;

// Forward declarations for card decks
class SkillCard;
class ChanceCard;
class GameEngine;

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
    const std::vector<Player*>& getAllPlayers() const;

    // current/max turn num
    // skillused, movedbycard, doublecount <- turncontext
};