#pragma once

#include <memory>
#include <string>
#include <vector>

#include "board/Board.hpp"
#include "core/TurnManager.hpp"
#include "core/TransactionLogger.hpp"
#include "card/DeckCard.hpp"
#include "card/ChanceCard.hpp"
#include "card/CommunityChestCard.hpp"
#include "card/SkillCard.hpp"

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
    CardDeck<CommunityChestCard> communityDeck;
    CardDeck<SkillCard> skillDeck;
    GameStatus status;
    TransactionLogger logger;

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

    // Bot turn execution
    bool executeBotTurn(TurnContext& ctx);
    void displayPlayers() const;
    std::vector<Player*> getPlayers() const;

    // Card deck methods
    void initializeCardDecks();
    ChanceCard* drawChanceCard();
    CommunityChestCard* drawCommunityChestCard();
    void returnChanceCard(ChanceCard* card);
    void returnCommunityChestCard(CommunityChestCard* card);

    // Transaction logger methods
    TransactionLogger& getLogger();
    void logAction(const std::string& user, const std::string& action, const std::string& detail);
    void printLogs(int n = -1);
};