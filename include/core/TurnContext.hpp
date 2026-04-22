#pragma once

class Player;
class Dice;
class Board;
class GameEngine;

class TurnContext {
public:
    Player& currentPlayer;
    Dice& dice;
    Board& board;
    GameEngine& gameEngine;
    int diceTotal;

    TurnContext(Player& player, Dice& dice, Board& board, GameEngine& gameEngine)
        : currentPlayer(player), dice(dice), board(board), gameEngine(gameEngine), diceTotal(0) {}

};