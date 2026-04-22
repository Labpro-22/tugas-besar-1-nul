#include "core/GameEngine.hpp"

#include "TurnContext.hpp"
#include "TurnManager.hpp"
#include "Command.hpp"
#include "Dice.hpp"

#include "board/Board.hpp"
#include "player/Player.hpp"
#include "card/DeckCard.hpp"
#include "card/ChanceCard.hpp"
#include "exception/CommandException.hpp"
#include "card/SkillCard.hpp"


void GameEngine::run() {
    std::cout << "=== Welcome to Nimonspoli ===" << endl;
    this->startNewGame();

    

    while (!turnmgr.isGameOver()) {
        Dice turnDice;
        TurnContext ctx(*turnmgr.getCurrentPlayer(), turnDice, board, *this);
        Player* currentPlayer = turnmgr.getCurrentPlayer();
        std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1) << ": " << currentPlayer->getUsername() << " ===\n";
        
        try {
            this->executeCommand(ctx);
        } catch (CommandException exc) {
            std::cout << exc.what() << "\n";
        }
        turnmgr.nextTurn();
    }

}

void GameEngine::loadGame(const std::string& file) {
    std::cout << "[INFO] Memuat konfigurasi board dari file: " << file << "\n";
    this->board = Board(40); // coba default dulu

    int maxTurns;
    std::cout << "[INFO] Memuat giliran maksimal permainan\n";
    std::cout << "Masukkan batas jumlah giliran permainan (atau -1 untuk tanpa batas)\n";
    std::cout << "> ";
    std::cin >> maxTurns;
    std::cin.ignore();
    this->turnmgr = TurnManager(maxTurns);

    int numPlayers;
    std::cout << "[INFO] Memuat pemain\n";
    std::cout << "Masukkan jumlah pemain (2-6)\n";
    std::cout << "> ";
    std::cin >> numPlayers;
    std::cin.ignore();
    for (int i = 0; i < numPlayers; ++i) {
        std::string username;
        std::cout << "Masukkan nama pemain " << (i + 1) << ": ";
        std::getline(std::cin, username);
        players.push_back(std::make_unique<Player>(username, 1500)); // saldo awal 1500
    }
    turnmgr.setTurnOrder(this->getPlayers());

}

void GameEngine::startNewGame() {
    std::cout << "[INFO] Memulai permainan baru...\n";
    std::cout << "Masukkan name file config board (atau tekan Enter untuk default)\n" ;
    std::cout << "> ";
    std::cin.ignore();
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) {
        this->loadGame("default_config.txt");
    } else {
        this->loadGame(filename);
    }
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

void GameEngine::executeCommand(TurnContext& ctx) {
    std::cout << "[INFO] Eksekusi perintah\n";

    Command cmd;
    cmd.promptInput();
    cmd.dispatch(ctx);
}