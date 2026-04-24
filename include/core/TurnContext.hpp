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

    // Flags untuk validasi giliran
    bool hasRolled;                 // Sudah lempar dadu?
    bool hasTakenJailAction;        // Sudah bayar/lempar dadu saat di penjara?

    TurnContext(Player& player, Dice& dice, Board& board, GameEngine& gameEngine)
        : currentPlayer(player), dice(dice), board(board), gameEngine(gameEngine),
          hasRolled(false), hasTakenJailAction(false) {}


    Tile& getTile() const;
    int getBoardSize() const;

    TurnManager& getTurnMgr() const;
    const std::vector<Player*> getAllPlayers() const;

    // Card deck methods
    ChanceCard* drawChanceCard();
    CommunityChestCard* drawCommunityChestCard();
    void returnChanceCard(ChanceCard* card);
    void returnCommunityChestCard(CommunityChestCard* card);

    // Validasi apakah bisa end turn
    bool canEndTurn() const;
};