#pragma once

#include <memory>
#include <string>
#include <vector>

class Board;
class Player;
class TurnManager;
class TurnContext;

template <typename T>
class CardDeck;
class ChanceCard;
class SkillCard;

class GameEngine {
private:
    Board board;
    TurnManager turnmgr;
    CardDeck<ChanceCard> chanceDeck;
    // CardDeck<CommunityCard> communityDeck;
    CardDeck<SkillCard> skillDeck;

    std::vector<std::unique_ptr<Player>> players;

public:
    explicit GameEngine(int size)
        : board(Board{size})
        , turnmgr(TurnManager{})
        , chanceDeck(CardDeck<ChanceCard>{})
        , skillDeck(CardDeck<SkillCard>{})
        , players{std::vector<std::unique_ptr<Player>>{}} {};

    void startNewGame();
    void run();
    void loadGame(const std::string& file);
    void saveGame(const std::string& file);
    void executeCommand(TurnContext& ctx);

    void displayPlayers() const;
    std::vector<Player*> getPlayers() const;
};