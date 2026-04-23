#include "core/GameEngine.hpp"

#include "core/TurnContext.hpp"
#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/ConfigLoader.hpp"
#include "tile/ActionTile.hpp"

#include "card/MoveCard.hpp"
#include "card/DiscountCard.hpp"
#include "card/ShieldCard.hpp"
#include "card/TeleportCard.hpp"
#include "card/LassoCard.hpp"
#include "card/DemolitionCard.hpp"

#include "player/Player.hpp"
#include "exception/CommandException.hpp"

#include <limits>

GameEngine::GameEngine(int size)
    : board(Board{size})
        , turnmgr(TurnManager{})
        , chanceDeck(CardDeck<ChanceCard>{})
        , skillDeck(CardDeck<SkillCard>{})
        , status(GameStatus::NOT_STARTED)
        , activeConfig(Config{})
        , players{std::vector<std::unique_ptr<Player>>{}} {

    skillDeck.addCard(new MoveCard(1));
    skillDeck.addCard(new MoveCard(2));
    skillDeck.addCard(new MoveCard(3));
    skillDeck.addCard(new DiscountCard(25));
    skillDeck.addCard(new DiscountCard(50));
    skillDeck.addCard(new ShieldCard());
    skillDeck.addCard(new TeleportCard(0));
    skillDeck.addCard(new LassoCard());
    skillDeck.addCard(new DemolitionCard());
    skillDeck.shuffle();
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
        
        goNext = false;
        try {
            cmd.promptInput();
            goNext = cmd.dispatch(ctx);
        } catch (const CommandException& exc) {
            std::cout << exc.what() << "\n";
        } catch (const std::exception& exc) {
            std::cout << exc.what() << "\n";
        }
        
        if (goNext) {
            turnmgr.nextTurn(ctx);
        }
    }

    std::cout << "\n=== Permainan Selesai! ===\n";
}

void GameEngine::loadGame() {
    std::cout << "[INFO] Loading Game...\n";
    std::cout << "Input configuration folder\n" ;
    std::cout << "> ";
    std::string folder;
    std::getline(std::cin, folder);
    if (folder.empty()) {
        this->board.generateDefaultBoard();
        return;
    }

    std::cout << "\n";
    std::cout << "[INFO] Loading configuration from folder: " << folder << "\n";
    
    try {
        ConfigLoader loader;
        Config cfg = loader.loadAll(folder);
        this->activeConfig = cfg;

        this->board = Board(40);
        this->board.buildFromConfig(this->activeConfig);
    } catch (const std::exception& ex) {
        std::cout << ex.what() << "\n";
        this->board.generateDefaultBoard();
    }

    std::cout << "[INFO] Board loaded from config successfully.\n";
    
    int maxTurns;
    std::cout << "[INFO] Querying max turns\n";
    std::cout << "Enter max turn (-1 for unlimited turns)\n";
    std::cout << "> ";
    std::cin >> maxTurns;
    if (maxTurns == -1 && this->activeConfig.misc.maxTurn > 0) {
        maxTurns = this->activeConfig.misc.maxTurn;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    this->turnmgr = TurnManager(maxTurns);

    int numPlayers;
    std::cout << "\n";
    std::cout << "[INFO] Querying players\n";
    std::cout << "Enter player amount (2-6)\n";
    std::cout << "> ";
    std::cin >> numPlayers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < numPlayers; ++i) {
        std::string username;
        std::cout << "Enter player " << (i + 1) << "'s name: ";
        std::getline(std::cin, username);
        const int startingBalance = (this->activeConfig.misc.startingBalance > 0)
            ? this->activeConfig.misc.startingBalance
            : 1500;
        players.push_back(std::make_unique<Player>(username, startingBalance));
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
    for (int i = 0; i < numPlayers; ++i) {
        std::string username;
        std::cout << "Enter player " << (i + 1) << "'s name: ";
        std::getline(std::cin, username);
        players.push_back(std::make_unique<Player>(username, 1500)); // saldo awal 1500
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

int GameEngine::getGoSalary() const {
    return (activeConfig.special.goSalary > 0) ? activeConfig.special.goSalary : 200;
}

int GameEngine::getStartingBalance() const {
    return (activeConfig.misc.startingBalance > 0) ? activeConfig.misc.startingBalance : 1500;
}

void GameEngine::giveRandomSkillCardTo(Player& player) {
    if (player.getHandsAmount() >= 3) {
        return;
    }

    if (skillDeck.size() == 0) {
        return;
    }

    SkillCard* drawn = skillDeck.draw();
    if (drawn == nullptr) {
        return;
    }

    player.drawSCard(drawn);
}

