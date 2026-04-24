#include "core/GameEngine.hpp"

#include "core/TurnContext.hpp"
#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/SaveLoadManager.hpp"
#include "tile/ActionTile.hpp"
#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"

#include "player/Player.hpp"
#include "player/BotPlayer.hpp"
#include "exception/CommandException.hpp"
#include "card/ChanceCard.hpp"
#include "card/CommunityChestCard.hpp"
#include "card/SkillCard.hpp"
#include "card/DemolitionCard.hpp"
#include "card/DiscountCard.hpp"
#include "card/LassoCard.hpp"
#include "card/MoveCard.hpp"
#include "card/ShieldCard.hpp"
#include "card/TeleportCard.hpp"
#include "states/GameState.hpp"
#include "states/PlayerState.hpp"
#include "states/CardState.hpp"
#include "board/Board.hpp"
#include "tile/Tile.hpp"

#include <limits>
#include <fstream>
#include <map>
#include <cstdlib>
#include <ctime>

GameEngine::GameEngine(int size)
    : board(Board{size})
        , turnmgr(TurnManager{})
        , chanceDeck(CardDeck<ChanceCard>{})
        , communityDeck(CardDeck<CommunityChestCard>{})
        , skillDeck(CardDeck<SkillCard>{})
        , status(GameStatus::NOT_STARTED)
        , players{std::vector<std::unique_ptr<Player>>{}} {
    initializeCardDecks();
}

GameEngine::~GameEngine() = default;

void GameEngine::run() {
    this->printBanner();
    this->startMenu();

    Dice gameDice;
    Command cmd;
    bool goNext;

    while (!turnmgr.isGameOver()) {
        Player* currentPlayer = turnmgr.getCurrentPlayer();
        if (currentPlayer == nullptr) {
            std::cout << "[WARN] No active player found. Stopping game loop.\n";
            break;
        }

        TurnContext ctx(*currentPlayer, gameDice, board, *this);
        std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1) << ": " << currentPlayer->getUsername() << " ===\n";

        // Check if current player is a bot
        if (currentPlayer->isBot()) {
            std::cout << "[BOT] " << currentPlayer->getUsername() << " sedang berpikir...\n";
            goNext = executeBotTurn(ctx);
        } else {
            // Human player - wait for command input
            goNext = false;
            try {
                cmd.promptInput();
                goNext = cmd.dispatch(ctx);
            } catch (const CommandException& exc) {
                std::cout << exc.what() << "\n";
            } catch (const std::exception& exc) {
                std::cout << exc.what() << "\n";
            }
        }

        if (goNext) {
            turnmgr.nextTurn(ctx);
        }
    }

    std::cout << "\n=== Permainan Selesai! ===\n";
}

bool GameEngine::executeBotTurn(TurnContext& ctx) {
    BotPlayer* bot = dynamic_cast<BotPlayer*>(&ctx.currentPlayer);
    if (bot == nullptr) {
        return false;
    }

    // Mark that bot has taken action (for save restriction)
    turnmgr.markActionTaken();

    // Bot rolls dice automatically
    std::cout << "[BOT] " << bot->getUsername() << " melempar dadu...\n";

    bool validRoll = ctx.dice.roll();
    if (!validRoll) {
        std::cout << "[WARN] Bot cannot roll the dice anymore this turn.\n";
        return true; // End turn
    }

    int diceTotal = ctx.dice.getTotal();
    std::cout << "[BOT] Hasil: " << ctx.dice.getDie1() << "+" << ctx.dice.getDie2() << " = " << diceTotal << "\n";

    // Log dice roll
    std::string logDetail = "Lempar: " + std::to_string(ctx.dice.getDie1()) + "+" +
                            std::to_string(ctx.dice.getDie2()) + "=" + std::to_string(diceTotal);
    logAction(bot->getUsername(), "DADU", logDetail);

    // Move bot
    int nextPos = bot->move(diceTotal, ctx);
    Tile* baseTile = board.getTile(nextPos);
    if (baseTile == nullptr) {
        std::cout << "[ERROR] Bot moved to invalid tile.\n";
        return true;
    }

    std::cout << "[BOT] " << bot->getUsername() << " mendarat di " << baseTile->getName() << "\n";

    // Log movement
    logDetail = "Mendarat di " + baseTile->getName() + " (" + baseTile->getCode() + ")";
    logAction(bot->getUsername(), "GERAK", logDetail);

    // Handle property tile
    PropertyTile* propTile = dynamic_cast<PropertyTile*>(baseTile);
    if (propTile != nullptr) {
        Property* prop = propTile->getProperty();
        if (prop != nullptr && prop->getStatus() == PropertyStatus::BANK) {
            // Bot decides whether to buy
            if (bot->decideBuyProperty(prop->getBuyPrice(), bot->getBalance())) {
                try {
                    bot->buy(prop);
                    std::cout << "[BOT] " << bot->getUsername() << " membeli " << prop->getName() << " seharga M" << prop->getBuyPrice() << "\n";
                    logAction(bot->getUsername(), "BELI", "Beli " + prop->getName() + " (" + prop->getCode() + ") seharga M" + std::to_string(prop->getBuyPrice()));
                } catch (const std::exception& e) {
                    std::cout << "[BOT] Gagal membeli: " << e.what() << "\n";
                }
            } else {
                std::cout << "[BOT] " << bot->getUsername() << " memutuskan untuk tidak membeli.\n";
            }
        }
    }

    // Trigger tile onLanded
    baseTile->onLanded(ctx);

    // Small delay for readability
    std::cout << "[BOT] Giliran " << bot->getUsername() << " selesai.\n";

    return true; // End bot's turn
}

void GameEngine::loadGame() {
    std::cout << "[INFO] Loading Game...\n";
    std::cout << "Pilih opsi:\n";
    std::cout << "  1. Load dari file save game\n";
    std::cout << "  2. Load dari konfigurasi board (default)\n";
    std::cout << "> ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        // Load dari file save game
        std::cout << "Masukkan nama file save game: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (!filename.empty()) {
            try {
                this->board.generateDefaultBoard();
                loadGame(filename);
                return;
            } catch (const std::exception& e) {
                std::cout << "[ERROR] Gagal load game: " << e.what() << "\n";
                std::cout << "Menggunakan konfigurasi default...\n";
            }
        }
    }
    
    // Load dari konfigurasi board (default behavior)
    std::cout << "Input configuration filename\n" ;
    std::cout << "> ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) {
        this->board.generateDefaultBoard();
        return;
    }

    std::cout << "\n";
    std::cout << "[INFO] Memuat konfigurasi board dari file: " << filename << "\n";
    this->board = Board(40); // temporary dummy board bootstrap
    // load from file
    this->board.generateDefaultBoard(); 

    int maxTurns;
    std::cout << "\n";
    std::cout << "[INFO] Memuat giliran maksimal permainan\n";
    std::cout << "Masukkan batas jumlah giliran permainan (atau -1 untuk tanpa batas)\n";
    std::cout << "> ";
    std::cin >> maxTurns;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    this->turnmgr = TurnManager(maxTurns);

    int numPlayers;
    std::cout << "\n";
    std::cout << "[INFO] Memuat pemain\n";
    std::cout << "Masukkan jumlah pemain (2-6)\n";
    std::cout << "> ";
    std::cin >> numPlayers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    int numBots = 0;
    std::cout << "Masukkan jumlah bot (0-" << numPlayers << "): ";
    std::cin >> numBots;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (numBots < 0) numBots = 0;
    if (numBots > numPlayers) numBots = numPlayers;
    
    int humanPlayers = numPlayers - numBots;
    
    // Create human players
    for (int i = 0; i < humanPlayers; ++i) {
        std::string username;
        std::cout << "Masukkan nama pemain " << (i + 1) << ": ";
        std::getline(std::cin, username);
        players.push_back(std::make_unique<Player>(username, 1500)); // saldo awal 1500
        std::cout << "[INFO] Added human player: " << username << "\n";
    }
    
    // Create bot players
    for (int i = 0; i < numBots; ++i) {
        std::string botName = "Bot" + std::to_string(i + 1);
        players.push_back(std::make_unique<BotPlayer>(botName, 1500));
        std::cout << "[INFO] Added bot player: " << botName << "\n";
    }
    
    turnmgr.setTurnOrder(this->getPlayers());
}

void GameEngine::newGame() {
    std::cout << "[INFO] Generating default map\n\n";
    this->board.generateDefaultBoard();

    int maxTurns;
    std::cout << "[INFO] Querying max turns\n";
    std::cout << "Enter max turn (-1 for unlimited turns)\n";
    std::cout << "> ";
    std::cin >> maxTurns;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    this->turnmgr = TurnManager(maxTurns);

    int numPlayers;
    std::cout << "\n";
    std::cout << "[INFO] Querying players\n";
    std::cout << "Enter player amount (2-6)\n";
    std::cout << "> ";
    std::cin >> numPlayers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    int numBots;
    std::cout << "Enter number of bots (0-" << numPlayers << "): ";
    std::cin >> numBots;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (numBots < 0) numBots = 0;
    if (numBots > numPlayers) numBots = numPlayers;
    
    int numHumans = numPlayers - numBots;
    
    // Create human players
    for (int i = 0; i < numHumans; ++i) {
        std::string username;
        std::cout << "Enter player " << (i + 1) << "'s name: ";
        std::getline(std::cin, username);
        players.push_back(std::make_unique<Player>(username, 1500)); // saldo awal 1500
        std::cout << "[INFO] Added human player: " << username << "\n";
    }
    
    // Create bot players
    for (int i = 0; i < numBots; ++i) {
        std::string botName = "Bot" + std::to_string(i + 1);
        players.push_back(std::make_unique<BotPlayer>(botName, 1500));
        std::cout << "[INFO] Added bot player: " << botName << "\n";
    }
    
    turnmgr.setTurnOrder(this->getPlayers());
}

// ini pake pointer, tapi gameengine pake unique_ptr,
// jdi mesti cek lagi apakah smuany pen diubah jdi unique_ptr
// ato ga
std::vector<Player*> GameEngine::getPlayers() const {
    std::vector<Player*> result;
    result.reserve(players.size());
    for (const auto& player : players) {
        result.push_back(player.get());
    }
    return result;
}


void GameEngine::displayPlayers() const {
    std::cout << "\n=== Daftar Pemain ===\n";
    for (const auto& player : players) {
        std::cout << "Pemain: " << player->getUsername() << " | Posisi: " << player->getPosition() << " | Kekayaan: " << player->getWealth() << "\n";
    }
    std::cout << "=====================\n\n";
}

void GameEngine::printBanner() {
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                              ║\n";
    cout << "  ║     ███╗   ██╗██╗███╗   ███╗ ██████╗ ███╗   ██╗███████╗      ║\n";
    cout << "  ║     ████╗  ██║██║████╗ ████║██╔═══██╗████╗  ██║██╔════╝      ║\n";
    cout << "  ║     ██╔██╗ ██║██║██╔████╔██║██║   ██║██╔██╗ ██║███████╗      ║\n";
    cout << "  ║     ██║╚██╗██║██║██║╚██╔╝██║██║   ██║██║╚██╗██║╚════██║      ║\n";
    cout << "  ║     ██║ ╚████║██║██║ ╚═╝ ██║╚██████╔╝██║ ╚████║███████║      ║\n";
    cout << "  ║     ╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝      ║\n";
    cout << "  ║                                                              ║\n";
    cout << "  ║              ██████╗  ██████╗ ██╗     ███████╗               ║\n";
    cout << "  ║              ██╔══██╗██╔═══██╗██║     ╚═███╔═╝               ║\n";
    cout << "  ║              ██████╔╝██║   ██║██║       ███║                 ║\n";
    cout << "  ║              ██╔═══╝ ██║   ██║██║       ███║                 ║\n";
    cout << "  ║              ██║     ╚██████╔╝███████╗███████╗               ║\n";
    cout << "  ║              ╚═╝      ╚═════╝ ╚══════╝╚══════╝               ║\n";
    cout << "  ║                                                              ║\n";
    cout << "  ╚══════════════════════════════════════════════════════════════╝\n";
}

void GameEngine::startMenu() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "             WELCOME TO\n";
    std::cout << "         N I M O N S P O L I\n";
    std::cout << "========================================\n";
    std::cout << "Pick an option:\n";
    std::cout << "  1. NEW GAME  - Start a new game\n";
    std::cout << "  2. LOAD GAME - Load a game\n";
    std::cout << "  3. HELP      - Show the help display\n";
    std::cout << "  4. EXIT      - Exit the game\n";

    while (true) {
        std::cout << "Enter a choice (1-4): ";
        std::string choiceLine;
        std::getline(std::cin, choiceLine);

        int choice = 0;
        try {
            choice = std::stoi(choiceLine);
        } catch (const std::exception&) {
            std::cout << "Input has to be 1-4. Please retry\n\n";
            continue;
        }

        if (choice == 1) {
            this->newGame();
            break;
        } else if (choice == 2) {
            this->loadGame();
            break;
        } else if (choice == 3) {
            break;
        } else if (choice == 4) {
            std::cout << "Thank you for playing!\n";
            exit(0);
            break;
        } else {
            std::cout << "Invalid choice. Please retry\n\n";
        }
    }
}

TurnManager& GameEngine::getTurnManager() {
    return turnmgr;
}

void GameEngine::initializeCardDecks() {
    // Initialize Chance Cards (Kesempatan)
    chanceDeck.addCard(new ChanceCard("Pergi ke stasiun terdekat", ChanceType::GO_TO_NEAREST_STATION));
    chanceDeck.addCard(new ChanceCard("Mundur 3 petak", ChanceType::MOVE_BACK_3));
    chanceDeck.addCard(new ChanceCard("Masuk Penjara", ChanceType::GO_TO_JAIL));
    chanceDeck.shuffle();

    // Initialize Community Chest Cards (Dana Umum)
    communityDeck.addCard(new CommunityChestCard("Ulang Tahun! Dapat M100 dari setiap pemain", CommunityChestType::BIRTHDAY, 100));
    communityDeck.addCard(new CommunityChestCard("Biaya Dokter. Bayar M700", CommunityChestType::DOCTOR_FEE, 700));
    communityDeck.addCard(new CommunityChestCard("Kampanye Politik. Bayar M200 ke setiap pemain", CommunityChestType::POLITICAL_CAMPAIGN, 200));
    communityDeck.shuffle();
}

ChanceCard* GameEngine::drawChanceCard() {
    if (chanceDeck.size() == 0) {
        chanceDeck.refillFromDiscard();
    }
    ChanceCard* card = chanceDeck.draw();
    if (card != nullptr) {
        std::cout << "[INFO] Mengambil kartu Kesempatan dari deck.\n";
    }
    return card;
}

CommunityChestCard* GameEngine::drawCommunityChestCard() {
    if (communityDeck.size() == 0) {
        communityDeck.refillFromDiscard();
    }
    CommunityChestCard* card = communityDeck.draw();
    if (card != nullptr) {
        std::cout << "[INFO] Mengambil kartu Dana Umum dari deck.\n";
    }
    return card;
}

void GameEngine::returnChanceCard(ChanceCard* card) {
    if (card != nullptr) {
        chanceDeck.addToDiscard(card);
    }
}

void GameEngine::returnCommunityChestCard(CommunityChestCard* card) {
    if (card != nullptr) {
        communityDeck.addToDiscard(card);
    }
}

TransactionLogger& GameEngine::getLogger() {
    return logger;
}

void GameEngine::logAction(const std::string& user, const std::string& action, const std::string& detail) {
    logger.log(turnmgr.getCurrentTurn(), user, action, detail);
}

void GameEngine::printLogs(int n) {
    logger.printLogs(n);
}

// Helper function to create skill card from CardState
static SkillCard* createSkillCardFromState(const CardState& cardState) {
    if (cardState.type == "DemolitionCard") {
        return new DemolitionCard();
    } else if (cardState.type == "DiscountCard") {
        return new DiscountCard(cardState.value);
    } else if (cardState.type == "LassoCard") {
        return new LassoCard();
    } else if (cardState.type == "MoveCard") {
        return new MoveCard(cardState.value);
    } else if (cardState.type == "ShieldCard") {
        return new ShieldCard();
    } else if (cardState.type == "TeleportCard") {
        return new TeleportCard(cardState.value);
    }
    return nullptr;
}

// Helper function to convert PlayerStatus to string
static std::string playerStatusToString(PlayerStatus status) {
    switch (status) {
        case PlayerStatus::ACTIVE: return "ACTIVE";
        case PlayerStatus::JAILED: return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
        default: return "ACTIVE";
    }
}

// Helper function to convert string to PlayerStatus
static PlayerStatus stringToPlayerStatus(const std::string& status) {
    if (status == "JAILED") return PlayerStatus::JAILED;
    if (status == "BANKRUPT") return PlayerStatus::BANKRUPT;
    return PlayerStatus::ACTIVE;
}

void GameEngine::loadGame(const std::string& file) {
    std::cout << "[INFO] Loading game from file: " << file << "\n";

    SaveLoadManager saveLoadManager;
    GameState state = saveLoadManager.load(file);

    // Clear existing players
    players.clear();

    // Create players based on saved state
    for (const PlayerState& ps : state.players) {
        std::unique_ptr<Player> player;

        if (ps.isBot) {
            // Create BotPlayer
            player = std::make_unique<BotPlayer>(ps.username, ps.balance);
            std::cout << "[INFO] Loaded bot player: " << ps.username << "\n";
        } else {
            // Create regular Player
            player = std::make_unique<Player>(ps.username, ps.balance);
            std::cout << "[INFO] Loaded player: " << ps.username << "\n";
        }

        // Set player state
        player->setPosition(board.getTileByCode(ps.positionCode) != nullptr ?
            board.getTileByCode(ps.positionCode)->getIndex() : 0);

        // Set status based on saved state
        if (ps.status == "JAILED") {
            player->enterJail();
        } else if (ps.status == "BANKRUPT") {
            player->setBankruptStatus();
        }

        // Add skill cards to hand
        for (const CardState& cardState : ps.hand) {
            SkillCard* card = createSkillCardFromState(cardState);
            if (card != nullptr) {
                player->drawSCard(card);
            }
        }

        players.push_back(std::move(player));
    }

    // Set up turn manager
    turnmgr = TurnManager(state.maxTurn);
    turnmgr.setTurnOrder(this->getPlayers());

    // Set current turn and active player index from saved state
    turnmgr.setCurrentTurn(state.currentTurn);
    turnmgr.setActivePlayerIndex(state.activePlayerIdx);

    // Copy log from state to TransactionLogger
    for (const auto& entry : state.log) {
        logger.log(entry.turn, entry.username, entry.actionType, entry.detail);
    }

    // Log the load action itself
    logger.log(turnmgr.getCurrentTurn(), "SYSTEM", "LOAD", "Game loaded from " + file);

    std::cout << "[INFO] Game loaded successfully!\n";
    std::cout << "[INFO] Current turn: " << state.currentTurn << "/" << state.maxTurn << "\n";
    if (state.activePlayerIdx >= 0 && state.activePlayerIdx < static_cast<int>(players.size())) {
        std::cout << "[INFO] Active player: " << players[state.activePlayerIdx]->getUsername() << "\n";
    }
}

void GameEngine::saveGame(const std::string& file) {
    std::cout << "[INFO] Saving game to file: " << file << "\n";

    GameState state;
    state.currentTurn = turnmgr.getCurrentTurn();
    state.maxTurn = turnmgr.getMaxTurn();

    // Build player states
    for (const auto& player : players) {
        PlayerState ps;
        ps.username = player->getUsername();
        ps.balance = player->getBalance();

        // Get position code from board
        Tile* tile = board.getTile(player->getPosition());
        ps.positionCode = tile != nullptr ? tile->getCode() : "GO";

        // Get status
        ps.status = playerStatusToString(player->getStatus());

        // Check if bot
        ps.isBot = player->isBot();

        // Save hand cards
        for (SkillCard* card : player->getHand()) {
            CardState cs;
            cs.type = card->getTypeName();
            cs.value = card->getSkillValue();
            cs.remainingDuration = card->getRemainingDuration();
            ps.hand.push_back(cs);
        }

        state.players.push_back(ps);
    }

    // Build turn order
    for (Player* player : turnmgr.getTurnOrder()) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].get() == player) {
                state.turnOrder.push_back(static_cast<int>(i));
                break;
            }
        }
    }

    // Set active player index
    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer != nullptr) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].get() == currentPlayer) {
                state.activePlayerIdx = static_cast<int>(i);
                break;
            }
        }
    }

    // Copy log from TransactionLogger to state
    state.log = logger.getAll();

    // Log the save action itself
    logger.log(turnmgr.getCurrentTurn(), "SYSTEM", "SAVE", "Game saved to " + file);

    // Save using SaveLoadManager
    SaveLoadManager saveLoadManager;
    saveLoadManager.save(state, file);

    std::cout << "[INFO] Game saved successfully!\n";
}

