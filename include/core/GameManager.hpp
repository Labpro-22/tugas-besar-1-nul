#pragma once

#include <vector>
#include <string>
#include <memory>

#include "config/Config.hpp"
#include "core/Dice.hpp"
#include "core/TurnContext.hpp"
#include "core/AuctionManager.hpp"
#include "core/TransactionLogger.hpp"
#include "board/Board.hpp"

// Forward declarations
class Player;
class Property;
class Tile;
class SkillCard;
class StreetTile;
class RailroadTile;
class UtilityTile;
class GoTile;
class JailTile;
class FreeParkingTile;
class GoToJailTile;
class CardTile;
class FestivalTile;
class TaxTile;
enum class TaxType;

// Game status enum
enum class GameStatus {
    NOT_STARTED,
    RUNNING,
    FINISHED
};

class GameManager {
private:
    // Configuration
    Config config;
    
    // Game state
    GameStatus status;
    std::vector<Player*> players;
    std::vector<int> turnOrder;
    int currentTurnNumber;
    int currentPlayerIndex;
    Board* board;
    
    // Components
    Dice dice;
    AuctionManager auctionManager;
    TransactionLogger logger;
    TurnContext* currentContext;
    
    // Double tracking
    int consecutiveDoubles;
    
    // Skill card deck
    std::vector<SkillCard*> skillDeck;
    std::vector<SkillCard*> skillDiscardPile;
    
    // Tiles
    std::vector<Tile*> tiles;

public:
    GameManager();
    ~GameManager();
    
    // Initialization
    void initialize(Config cfg);
    void setupNewGame(int numPlayers, const std::vector<std::string>& usernames);
    void setupLoadedGame(const std::vector<std::string>& usernames, 
                         const std::vector<int>& balances,
                         const std::vector<int>& positions,
                         const std::vector<std::string>& statuses,
                         int currentTurn, int currentPlayerIdx);
    
    // Game flow
    void startGame();
    void runGameLoop();
    void processTurn();
    bool nextTurn();
    void endGame();
    
    // Dice operations
    void rollDice();
    void setManualDice(int d1, int d2);
    int getLastDiceTotal() const;
    bool wasLastRollDouble() const;
    
    // Player movement
    void moveCurrentPlayer(int steps);
    void movePlayerToTile(int tileIndex);
    
    // Property operations
    void handlePropertyLanding(Property* prop);
    void buyProperty(Property* prop);
    void auctionProperty(Property* prop);
    void mortgageProperty(Property* prop);
    void redeemProperty(Property* prop);
    
    // Building operations
    void buildHouse(const std::string& propertyCode);
    void sellHouse(const std::string& propertyCode);
    void upgradeToHotel(const std::string& propertyCode);
    
    // Skill card operations
    void drawSkillCard();
    void useSkillCard(int cardIndex);
    void discardSkillCard(int cardIndex);
    
    // Jail operations
    void handleJailTurn();
    void payJailFine();
    void useJailCard();
    void tryJailRoll();
    
    // Bankruptcy
    void handleBankruptcy(Player* player, int debtAmount, Player* creditor);
    
    // Save/Load
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);
    
    // Getters
    GameStatus getStatus() const;
    Player* getCurrentPlayer() const;
    Board* getBoard() const;
    int getCurrentTurn() const;
    int getMaxTurn() const;
    const std::vector<Player*>& getAllPlayers() const;
    TransactionLogger& getLogger();
    Config& getConfig();
    
    // Display
    void printBoard();
    void printProperty(const std::string& code);
    void printPlayerProperties();
    void printLog(int count = 0);
    
    // Helper methods
    void distributeSkillCards();
    void checkGameEnd();
    void declareWinner();
    
private:
    void initializeBoard();
    void buildTiles();
    void initializeSkillDeck();
    void shuffleSkillDeck();
    void handleTileEffect(Tile* tile);
    void handleDoubleRoll();
    void resetTurnState();
    int calculateNextPlayerIndex();
    void removeBankruptPlayers();
};
