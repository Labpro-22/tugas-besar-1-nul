#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"

void GameEngine::run() {
    std::cout << "=== Welcome to Nimonspoli ===\n";
    this->startNewGame();

    // ==========================================
    // 1. Ekstrak raw pointer Player (Lakukan SEKALI saja di luar loop)
    // ==========================================
    std::vector<Player*> rawPlayers;
    for (const auto& p : this->players) { // Asumsi variabel penyimpan pemain bernama 'players'
        rawPlayers.push_back(p.get());
    }

    // ==========================================
    // 2. Loop Utama Permainan
    // ==========================================
    while (!turnmgr.isGameOver()) {
        Player* currentPlayer = turnmgr.getCurrentPlayer();
        int currentTurn = turnmgr.getCurrentTurn();
        int maxTurn = turnmgr.getMaxTurn();

        // Buat konteks dengan parameter yang BENAR
        TurnContext ctx(currentPlayer, &board, currentTurn, maxTurn, rawPlayers);

        std::cout << "\n=== Giliran " << currentTurn << ": " << currentPlayer->getUsername() << " ===\n";
        
        // TO-DO: player decide action
        // Contoh:
        // gameDice.roll();
        // currentPlayer->move(gameDice.getTotal(), ctx);
        
        // ==========================================
        // 3. Pindah Giliran (Wajib di BAWAH setelah pemain selesai)
        // ==========================================
        turnmgr.nextTurn(); 
    }
    
    std::cout << "\n=== Permainan Selesai! ===\n";
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

void GameEngine::executeCommand(const std::string& cmd) {
    std::cout << "[INFO] Eksekusi perintah: " << cmd << "\n";
    // TO-DO: mikir input tuh mau dimasukkin dimana
}