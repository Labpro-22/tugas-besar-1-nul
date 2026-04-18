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
    Board board;
    TurnManager turnmgr;
    CardDeck<ChanceCard> chanceDeck;
    // CardDeck<CommunityCard> communityDeck;
    CardDeck<SkillCard> skillDeck;

    // GameEngine owns all Player objects.
    std::vector<std::unique_ptr<Player>> players;

public:
    // ctor dtor lom ada
    GameEngine ge();

    void startNewGame();
    void loadGame(const std::string& file);
    void saveGame(const std::string& file);
    void executeCommand(const std::string& cmd);

    void displayPlayers() const;
    std::vector<Player*> getPlayers() const;
};