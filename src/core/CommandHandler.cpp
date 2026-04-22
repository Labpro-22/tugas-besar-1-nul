#include "core/CommandHandler.hpp"
#include "core/GameManager.hpp"
#include "player/Player.h"
#include "property/Property.hpp"
#include "property/StreetProperty.hpp"
#include "config/Config.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <cctype>

CommandHandler::CommandHandler(GameManager* game) 
    : game(game), running(false) {
}

void CommandHandler::displayMainMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "         SELAMAT DATANG DI              " << std::endl;
    std::cout << "         N I M O N S P O L I            " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nPilih opsi:" << std::endl;
    std::cout << "  1. NEW GAME - Mulai permainan baru" << std::endl;
    std::cout << "  2. LOAD GAME - Muat permainan tersimpan" << std::endl;
    std::cout << "  3. HELP - Tampilkan bantuan" << std::endl;
    std::cout << "  4. EXIT - Keluar" << std::endl;
    std::cout << "\nMasukkan pilihan (1-4): ";
}

void CommandHandler::runMainLoop() {
    running = true;
    std::string input;
    
    while (running && game->getStatus() == GameStatus::RUNNING) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (!processCommand(input)) {
            // Command returned false, might indicate quit
        }
    }
}

bool CommandHandler::processCommand(const std::string& command) {
    std::vector<std::string> tokens = tokenize(command);
    if (tokens.empty()) {
        return true;
    }
    
    std::string cmd = toUpper(tokens[0]);
    
    if (cmd == "CETAK_PAPAN" || cmd == "BOARD") {
        handleCetakPapan();
    } else if (cmd == "LEMPAR_DADU" || cmd == "ROLL" || cmd == "DADU") {
        handleLemparDadu();
    } else if (cmd == "ATUR_DADU" || cmd == "SET_DICE") {
        handleAturDadu(tokens);
    } else if (cmd == "CETAK_AKTA" || cmd == "AKTA") {
        handleCetakAkta(tokens);
    } else if (cmd == "CETAK_PROPERTI" || cmd == "PROPERTI" || cmd == "PROPERTY") {
        handleCetakProperti();
    } else if (cmd == "GADAI" || cmd == "MORTGAGE") {
        handleGadai();
    } else if (cmd == "TEBUS" || cmd == "REDEEM") {
        handleTebus();
    } else if (cmd == "BANGUN" || cmd == "BUILD") {
        handleBangun();
    } else if (cmd == "SIMPAN" || cmd == "SAVE") {
        handleSimpan(tokens);
    } else if (cmd == "MUAT" || cmd == "LOAD") {
        handleMuat(tokens);
    } else if (cmd == "CETAK_LOG" || cmd == "LOG") {
        handleCetakLog(tokens);
    } else if (cmd == "GUNAKAN_KEMAMPUAN" || cmd == "SKILL" || cmd == "KARTU") {
        handleGunakanKemampuan();
    } else if (cmd == "HELP" || cmd == "BANTUAN" || cmd == "?") {
        handleHelp();
    } else if (cmd == "EXIT" || cmd == "QUIT" || cmd == "KELUAR") {
        handleExit();
        return false;
    } else {
        std::cout << "Perintah tidak dikenal: " << tokens[0] << std::endl;
        std::cout << "Ketik HELP untuk daftar perintah." << std::endl;
    }
    
    return true;
}

void CommandHandler::handleCetakPapan() {
    game->printBoard();
}

void CommandHandler::handleLemparDadu() {
    if (game->getStatus() != GameStatus::RUNNING) {
        std::cout << "Permainan belum dimulai!" << std::endl;
        return;
    }
    
    std::cout << "Mengocok dadu..." << std::endl;
    game->rollDice();
}

void CommandHandler::handleAturDadu(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Penggunaan: ATUR_DADU <dadu1> <dadu2>" << std::endl;
        std::cout << "Contoh: ATUR_DADU 3 4" << std::endl;
        return;
    }
    
    try {
        int d1 = std::stoi(args[1]);
        int d2 = std::stoi(args[2]);
        
        if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
            std::cout << "Nilai dadu harus antara 1-6!" << std::endl;
            return;
        }
        
        game->setManualDice(d1, d2);
        std::cout << "Dadu diatur: " << d1 << " + " << d2 << " = " << (d1 + d2) << std::endl;
        game->rollDice();
        
        // Reset to random mode after use
        // (rollDice already uses the manual value)
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void CommandHandler::handleCetakAkta(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Penggunaan: CETAK_AKTA <kode_properti>" << std::endl;
        std::cout << "Contoh: CETAK_AKTA JKT" << std::endl;
        return;
    }
    
    game->printProperty(args[1]);
}

void CommandHandler::handleCetakProperti() {
    game->printPlayerProperties();
}

void CommandHandler::handleGadai() {
    Player* player = game->getCurrentPlayer();
    if (!player) {
        std::cout << "Tidak ada pemain aktif!" << std::endl;
        return;
    }
    
    const auto& properties = player->getProperties();
    if (properties.empty()) {
        std::cout << "Anda tidak memiliki properti!" << std::endl;
        return;
    }
    
    std::cout << "=== Properti yang Dapat Digadaikan ===" << std::endl;
    int count = 0;
    std::vector<Property*> mortgageable;
    
    for (Property* prop : properties) {
        if (prop->getStatus() == PropertyStatus::OWNED) {
            count++;
            mortgageable.push_back(prop);
            std::cout << count << ". " << prop->getName() 
                      << " (" << prop->getCode() << ") - Nilai Gadai: M" 
                      << prop->getMortgageValue() << std::endl;
        }
    }
    
    if (mortgageable.empty()) {
        std::cout << "Tidak ada properti yang dapat digadaikan." << std::endl;
        return;
    }
    
    std::cout << "\nPilih nomor properti (0 untuk batal): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice > 0 && choice <= static_cast<int>(mortgageable.size())) {
        Property* selected = mortgageable[choice - 1];
        try {
            player->mortgage(selected);
            std::cout << selected->getName() << " berhasil digadaikan!" << std::endl;
            std::cout << "Anda menerima M" << selected->getMortgageValue() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}

void CommandHandler::handleTebus() {
    Player* player = game->getCurrentPlayer();
    if (!player) {
        std::cout << "Tidak ada pemain aktif!" << std::endl;
        return;
    }
    
    const auto& properties = player->getProperties();
    std::vector<Property*> redeemable;
    
    for (Property* prop : properties) {
        if (prop->getStatus() == PropertyStatus::MORTGAGED) {
            redeemable.push_back(prop);
        }
    }
    
    if (redeemable.empty()) {
        std::cout << "Tidak ada properti yang sedang digadaikan." << std::endl;
        return;
    }
    
    std::cout << "=== Properti yang Sedang Digadaikan ===" << std::endl;
    int count = 0;
    for (Property* prop : redeemable) {
        count++;
        std::cout << count << ". " << prop->getName() 
                  << " (" << prop->getCode() << ") - Harga Tebus: M" 
                  << prop->getBuyPrice() << std::endl;
    }
    
    std::cout << "\nPilih nomor properti (0 untuk batal): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice > 0 && choice <= static_cast<int>(redeemable.size())) {
        Property* selected = redeemable[choice - 1];
        try {
            // TODO: Implement redeem in Property class
            std::cout << selected->getName() << " berhasil ditebus!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}

void CommandHandler::handleBangun() {
    std::cout << "=== Bangun Rumah/Hotel ===" << std::endl;
    std::cout << "Fitur ini akan diimplementasikan." << std::endl;
}

void CommandHandler::handleSimpan(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Penggunaan: SIMPAN <nama_file>" << std::endl;
        return;
    }
    
    game->saveGame(args[1]);
    std::cout << "Permainan disimpan ke: " << args[1] << std::endl;
}

void CommandHandler::handleMuat(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Penggunaan: MUAT <nama_file>" << std::endl;
        return;
    }
    
    game->loadGame(args[1]);
    std::cout << "Permainan dimuat dari: " << args[1] << std::endl;
}

void CommandHandler::handleCetakLog(const std::vector<std::string>& args) {
    if (args.size() > 1) {
        try {
            int count = std::stoi(args[1]);
            game->printLog(count);
        } catch (...) {
            game->printLog(0);
        }
    } else {
        game->printLog(0);
    }
}

void CommandHandler::handleGunakanKemampuan() {
    Player* player = game->getCurrentPlayer();
    if (!player) {
        std::cout << "Tidak ada pemain aktif!" << std::endl;
        return;
    }
    
    if (player->getHandsAmount() == 0) {
        std::cout << "Anda tidak memiliki kartu kemampuan!" << std::endl;
        return;
    }
    
    std::cout << "=== Kartu Kemampuan ===" << std::endl;
    player->showHands();
    
    std::cout << "\nPilih nomor kartu (0 untuk batal): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice > 0 && choice <= player->getHandsAmount()) {
        // TODO: Implement useSCard with proper TurnContext
        // player->useSCard(choice - 1, *game->getCurrentContext());
        std::cout << "Kartu digunakan!" << std::endl;
    }
}

void CommandHandler::handleHelp() {
    std::cout << "\n========== DAFTAR PERINTAH ==========" << std::endl;
    std::cout << "PERMAINAN:" << std::endl;
    std::cout << "  LEMPAR_DADU       - Lempar dadu" << std::endl;
    std::cout << "  ATUR_DADU x y     - Atur dadu manual (cheat)" << std::endl;
    std::cout << "  CETAK_PAPAN       - Tampilkan papan permainan" << std::endl;
    std::cout << std::endl;
    std::cout << "PROPERTI:" << std::endl;
    std::cout << "  CETAK_AKTA kode   - Lihat detail properti" << std::endl;
    std::cout << "  CETAK_PROPERTI    - Lihat properti milik Anda" << std::endl;
    std::cout << "  GADAI             - Gadai properti ke bank" << std::endl;
    std::cout << "  TEBUS             - Tebus properti yang digadaikan" << std::endl;
    std::cout << "  BANGUN            - Bangun rumah/hotel" << std::endl;
    std::cout << std::endl;
    std::cout << "KARTU:" << std::endl;
    std::cout << "  GUNAKAN_KEMAMPUAN - Gunakan kartu kemampuan" << std::endl;
    std::cout << std::endl;
    std::cout << "SISTEM:" << std::endl;
    std::cout << "  SIMPAN file       - Simpan permainan" << std::endl;
    std::cout << "  MUAT file         - Muat permainan" << std::endl;
    std::cout << "  CETAK_LOG [n]     - Tampilkan log transaksi" << std::endl;
    std::cout << "  HELP              - Tampilkan bantuan ini" << std::endl;
    std::cout << "  EXIT              - Keluar dari permainan" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void CommandHandler::handleExit() {
    std::cout << "Terima kasih telah bermain Nimonspoli!" << std::endl;
    running = false;
}

std::vector<std::string> CommandHandler::tokenize(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void CommandHandler::trim(std::string& str) {
    // Trim leading whitespace
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        str.clear();
        return;
    }
    
    // Trim trailing whitespace
    size_t end = str.find_last_not_of(" \t\n\r");
    str = str.substr(start, end - start + 1);
}

std::string CommandHandler::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}
