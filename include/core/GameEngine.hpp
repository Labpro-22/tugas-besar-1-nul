#pragma once

#include <memory>
#include <string>
#include <vector>

#include "board/Board.hpp"
#include "core/TurnManager.hpp"
#include "card/DeckCard.hpp"
#include "card/ChanceCard.hpp"
#include "card/SkillCard.hpp"
#include "config/Config.hpp"

class Player;
class TurnContext;

enum class GameStatus {
    NOT_STARTED,
    RUNNING,
    FINISHED
};

class GameEngine {
private:
    // Board board;
    Board board;
    TurnManager turnmgr;
    CardDeck<ChanceCard> chanceDeck;
    // CardDeck<CommunityCard> communityDeck;
    CardDeck<SkillCard> skillDeck;
    GameStatus status;
    Config activeConfig;

    std::vector<std::unique_ptr<Player>> players;

public:
    explicit GameEngine(int size);
    ~GameEngine();

    void newGame();
    void loadGame();
    void run();
    void loadGame(const std::string& file);
    void saveGame(const std::string& file);
    void executeCommand(TurnContext& ctx);

    void printBanner();
    void startMenu();
    
    TurnManager& getTurnManager();
    void displayPlayers() const;
    std::vector<Player*> getPlayers() const;

    int getGoSalary() const;
    int getStartingBalance() const;
    void giveRandomSkillCardTo(Player& player);
};