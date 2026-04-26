#pragma once

#include <string>
#include <vector>

#include "board/Board.hpp"
#include "core/TurnManager.hpp"
#include "core/TransactionLogger.hpp"
#include "card/DeckCard.hpp"
#include "card/ChanceCard.hpp"
#include "card/DeckCard.hpp"
#include "card/CommunityChestCard.hpp"
#include "card/SkillCard.hpp"
#include "config/Config.hpp"
#include "core/TurnManager.hpp"
#include "states/GameState.hpp"
#include "gui/GuiRenderer.hpp"
#include "gui/GuiPanelManager.hpp"

class Player;
class TurnContext;

enum class GameStatus { NOT_STARTED, RUNNING, FINISHED };

class GameEngine {
  private:
    Board board;
    TurnManager turnmgr;
    CardDeck<ChanceCard> chanceDeck;
    CardDeck<CommunityChestCard> communityDeck;
    CardDeck<SkillCard> skillDeck;
    GameStatus status;
    Config activeConfig;
    TransactionLogger logger;

    std::vector<Player*> players;
    void clearPlayers();

    GuiPanels::PanelManager* panelManager = nullptr;

  public:
    explicit GameEngine(int size);
    ~GameEngine();

    void newGame();
    void loadGameFromSave();
    void run();
    void runGUI();  // Run with GUI visualization
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

    int getConfiguredMaxTurn() const;
    int getGoSalary() const;
    int getJailFine() const;
    int getTaxPphFlat() const;
    int getTaxPphPercent() const;
    int getTaxPbmFlat() const;
    int getStartingBalance() const;
    void giveRandomSkillCardTo(Player& player);
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

    // Build GameState for GUI rendering
    GameState buildGameState() const;

    // Get board reference
    Board& getBoard();

    // GUI panel manager
    void setPanelManager(GuiPanels::PanelManager* pm);
    GuiPanels::PanelManager* getPanelManager() const;
};