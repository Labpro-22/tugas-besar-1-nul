#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "config/ConfigDriver.hpp"
#include "config/Config.hpp"
#include "core/GameManager.hpp"
#include "core/CommandHandler.hpp"

using namespace std;

void displayBanner() {
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                              ║\n";
    cout << "  ║   ███╗   ██╗██╗███╗   ███╗ ██████╗ ███╗   ██╗███████╗       ║\n";
    cout << "  ║   ████╗  ██║██║████╗ ████║██╔═══██╗████╗  ██║██╔════╝       ║\n";
    cout << "  ║   ██╔██╗ ██║██║██╔████╔██║██║   ██║██╔██╗ ██║███████╗       ║\n";
    cout << "  ║   ██║╚██╗██║██║██║╚██╔╝██║██║   ██║██║╚██╗██║╚════██║       ║\n";
    cout << "  ║   ██║ ╚████║██║██║ ╚═╝ ██║╚██████╔╝██║ ╚████║███████║       ║\n";
    cout << "  ║   ╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝       ║\n";
    cout << "  ║                                                              ║\n";
    cout << "  ║              ██████╗  ██████╗ ██╗     ██╗                   ║\n";
    cout << "  ║              ██╔══██╗██╔═══██╗██║     ██║                   ║\n";
    cout << "  ║              ██████╔╝██║   ██║██║     ██║                   ║\n";
    cout << "  ║              ██╔═══╝ ██║   ██║██║     ██║                   ║\n";
    cout << "  ║              ██║     ╚██████╔╝███████╗███████╗              ║\n";
    cout << "  ║              ╚═╝      ╚═════╝ ╚══════╝╚══════╝              ║\n";
    cout << "  ║                                                              ║\n";
    cout << "  ╚══════════════════════════════════════════════════════════════╝\n";
}

int getMenuChoice() {
    int choice;
    cout << "\n╔══════════════════════════════════════╗\n";
    cout << "║          M E N U   U T A M A         ║\n";
    cout << "╠══════════════════════════════════════╣\n";
    cout << "║  1. NEW GAME  - Mulai permainan baru ║\n";
    cout << "║  2. LOAD GAME - Muat permainan       ║\n";
    cout << "║  3. HELP      - Panduan permainan    ║\n";
    cout << "║  4. EXIT      - Keluar               ║\n";
    cout << "╚══════════════════════════════════════╝\n";
    cout << "\nMasukkan pilihan (1-4): ";
    
    while (!(cin >> choice) || choice < 1 || choice > 4) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Pilihan tidak valid. Masukkan angka 1-4: ";
    }
    cin.ignore(10000, '\n');
    
    return choice;
}

void displayHelp() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║                        P A N D U A N                             ║\n";
    cout << "╠══════════════════════════════════════════════════════════════════╣\n";
    cout << "║ Nimonspoli adalah permainan papan strategi yang terinspirasi    ║\n";
    cout << "║ dari Monopoly. Pemain bergerak mengelilingi papan, membeli       ║\n";
    cout << "║ properti, membangun rumah/hotel, dan memungut sewa.              ║\n";
    cout << "║                                                                  ║\n";
    cout << "║ Tujuan: Menjadi pemain terkaya atau satu-satunya yang tersisa   ║\n";
    cout << "║ setelah pemain lain bangkrut.                                    ║\n";
    cout << "║                                                                  ║\n";
    cout << "║ Dalam permainan, Anda dapat menggunakan perintah:                ║\n";
    cout << "║   - LEMPAR_DADU    : Lempar dadu untuk bergerak                  ║\n";
    cout << "║   - CETAK_PAPAN    : Lihat papan permainan                       ║\n";
    cout << "║   - CETAK_PROPERTI : Lihat properti Anda                         ║\n";
    cout << "║   - GADAI          : Gadai properti ke bank                      ║\n";
    cout << "║   - BANGUN         : Bangun rumah/hotel                          ║\n";
    cout << "║   - dan banyak lagi...                                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
}

vector<string> getPlayerNames() {
    int numPlayers;
    cout << "\nMasukkan jumlah pemain (2-4): ";
    while (!(cin >> numPlayers) || numPlayers < 2 || numPlayers > 4) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Jumlah pemain harus antara 2-4: ";
    }
    cin.ignore(10000, '\n');
    
    vector<string> names;
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        cout << "Masukkan nama pemain " << (i + 1) << ": ";
        getline(cin, name);
        
        // Ensure name is not empty
        while (name.empty()) {
            cout << "Nama tidak boleh kosong. Masukkan nama: ";
            getline(cin, name);
        }
        
        names.push_back(name);
    }
    
    return names;
}

void runNewGame(Config& config) {
    cout << "\n========== NEW GAME ==========\n";
    
    vector<string> playerNames = getPlayerNames();
    
    GameManager game;
    game.initialize(config);
    game.setupNewGame(playerNames.size(), playerNames);
    game.startGame();
    
    CommandHandler cmdHandler(&game);
    cmdHandler.runMainLoop();
}

void runLoadGame(Config& config) {
    cout << "\n========== LOAD GAME ==========\n";
    cout << "Fitur load game akan diimplementasikan sepenuhnya.\n";
    cout << "Untuk saat ini, silakan mulai permainan baru.\n";
}

int main(int argc, char* argv[]) {
    const string basePath = (argc >= 2) ? argv[1] : "config";
    
    displayBanner();
    
    // Load configuration
    Config config;
    try {
        cout << "\nMemuat konfigurasi dari: " << basePath << "...\n";
        // For now, use default config values
        // In the future, this should load from files
        config.misc.maxTurn = 15;
        config.misc.startingBalance = 1000;
        config.special.goSalary = 200;
        config.special.jailFine = 50;
        config.tax.pphFlat = 150;
        config.tax.pphPercent = 10;
        config.tax.pbmFlat = 200;
        cout << "Konfigurasi berhasil dimuat!\n";
    } catch (const exception& ex) {
        cerr << "[ERROR] Gagal memuat konfigurasi: " << ex.what() << endl;
        cerr << "Menggunakan konfigurasi default.\n";
    }
    
    bool running = true;
    while (running) {
        int choice = getMenuChoice();
        
        switch (choice) {
            case 1:
                runNewGame(config);
                break;
            case 2:
                runLoadGame(config);
                break;
            case 3:
                displayHelp();
                break;
            case 4:
                running = false;
                break;
        }
    }
    
    cout << "\nTerima kasih telah bermain Nimonspoli!\n";
    cout << "Sampai jumpa!\n\n";
    
    return 0;
}
