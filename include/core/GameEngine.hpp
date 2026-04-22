#pragma once

#include <memory>
#include <string>
#include <vector>

#include "board/Board.hpp"
#include "player/Player.h"
#include "card/DeckCard.hpp"
#include "card/ChanceCard.hpp"
// #include "card/CommunityCard.hpp"
#include "card/SkillCard.hpp"
#include "TurnManager.hpp"

class GameEngine {
private:
    // Board board;
    Board board{20};
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
    void executeCommand(const std::string& cmd);

    void displayPlayers() const;
    std::vector<Player*> getPlayers() const;
};