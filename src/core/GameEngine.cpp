#include "core/GameEngine.hpp"

#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/TurnContext.hpp"
#include "tile/ActionTile.hpp"

#include "exception/CommandException.hpp"
#include "player/Player.hpp"

#include <limits>

GameEngine::GameEngine(int size)
    : board(Board{size}), turnmgr(TurnManager{}),
      chanceDeck(CardDeck<ChanceCard>{}), skillDeck(CardDeck<SkillCard>{}),
      players{std::vector<std::unique_ptr<Player>>{}} {};

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
        std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1) << ": "
                  << currentPlayer->getUsername() << " ===\n";

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
    std::cout << "Input configuration filename\n";
    std::cout << "> ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) {
        this->board.generateDefaultBoard();
        return;
    }

    std::cout << "\n";
    std::cout << "[INFO] Memuat konfigurasi board dari file: " << filename
              << "\n";
    this->board = Board(40); // temporary dummy board bootstrap
    // load from file
    this->board.generateDefaultBoard();

    int maxTurns;
    std::cout << "\n";
    std::cout << "[INFO] Memuat giliran maksimal permainan\n";
    std::cout << "Masukkan batas jumlah giliran permainan (atau -1 untuk tanpa "
                 "batas)\n";
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
    for (int i = 0; i < numPlayers; ++i) {
        std::string username;
        std::cout << "Masukkan nama pemain " << (i + 1) << ": ";
        std::getline(std::cin, username);
        players.push_back(
            std::make_unique<Player>(username, 1500)); // saldo awal 1500
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
        players.push_back(
            std::make_unique<Player>(username, 1500)); // saldo awal 1500
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
        std::cout << "Pemain: " << player->getUsername()
                  << " | Posisi: " << player->getPosition()
                  << " | Kekayaan: " << player->getWealth() << "\n";
    }
    std::cout << "=====================\n\n";
}

void GameEngine::printBanner() {
    cout << "\n";
    cout
        << "  "
           "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                              "
            "║\n";
    cout << "  ║     ███╗   ██╗██╗███╗   ███╗ ██████╗ ███╗   ██╗███████╗      "
            "║\n";
    cout << "  ║     ████╗  ██║██║████╗ ████║██╔═══██╗████╗  ██║██╔════╝      "
            "║\n";
    cout << "  ║     ██╔██╗ ██║██║██╔████╔██║██║   ██║██╔██╗ ██║███████╗      "
            "║\n";
    cout << "  ║     ██║╚██╗██║██║██║╚██╔╝██║██║   ██║██║╚██╗██║╚════██║      "
            "║\n";
    cout << "  ║     ██║ ╚████║██║██║ ╚═╝ ██║╚██████╔╝██║ ╚████║███████║      "
            "║\n";
    cout << "  ║     ╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝      "
            "║\n";
    cout << "  ║                                                              "
            "║\n";
    cout << "  ║              ██████╗  ██████╗ ██╗     ███████╗               "
            "║\n";
    cout << "  ║              ██╔══██╗██╔═══██╗██║     ╚═███╔═╝               "
            "║\n";
    cout << "  ║              ██████╔╝██║   ██║██║       ███║                 "
            "║\n";
    cout << "  ║              ██╔═══╝ ██║   ██║██║       ███║                 "
            "║\n";
    cout << "  ║              ██║     ╚██████╔╝███████╗███████╗               "
            "║\n";
    cout << "  ║              ╚═╝      ╚═════╝ ╚══════╝╚══════╝               "
            "║\n";
    cout << "  ║                                                              "
            "║\n";
    cout
        << "  "
           "╚══════════════════════════════════════════════════════════════╝\n";
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
