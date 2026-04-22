#include "card/LassoCard.hpp"
#include "player/Player.h"
#include <iostream>
#include <limits>

LassoCard::LassoCard() : SkillCard("LassoCard: Menarik pemain lawan", 0) {}

void LassoCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] LassoCard: Menarik pemain lawan ke posisi ini.\n";

    // TODO: Tampilkan daftar pemain lain yang posisinya berada di "depan"
    std::cout << "\n=== Daftar Pemain yang Berada di Depan ===\n";
    std::cout << "Pemain saat ini: " << player.getUsername() << " (posisi: TODO)\n";
    std::cout << "TODO: Filter pemain yang berada di depan\n";
    std::cout << "TODO: Tampilkan list pemain target\n\n";
    
    // TODO: Minta input pemain mana yang ditarik
    std::cout << "Pilih pemain yang akan ditarik (nomor): ";
    int choice;
    std::cin >> choice;
    
    std::cout << "\n[INFO] TODO: Validasi pemain pilihan\n";
    std::cout << "[INFO] TODO: Pindahkan pemain lawan ke posisi: " << "TODO" << "\n";
    std::cout << "[INFO] TODO: Eksekusi onLanded untuk pemain lawan\n";
    
    // Bersihkan buffer input
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
