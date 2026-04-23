#include "card/LassoCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "board/Board.hpp"

#include <iostream>
#include <limits>
#include <vector>

LassoCard::LassoCard() : SkillCard("LassoCard: Menarik pemain lawan", 0) {}

void LassoCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    const int currentPos = player.getPosition();
    const int boardSize = ctx.getBoardSize();

    std::cout << "[MENGGUNAKAN] LassoCard: Menarik pemain lawan ke posisi ini.\n";

    std::cout << "\n=== Daftar Pemain yang Berada di Depan ===\n";
    std::cout << "Pemain saat ini: " << player.getUsername() << " (posisi: " << currentPos << ")\n";

    std::vector<Player*> candidates;
    const std::vector<Player*> allPlayers = ctx.getAllPlayers();
    for (Player* other : allPlayers) {
        if (other == nullptr || other == &player) continue;

        const int forwardDistance = (other->getPosition() - currentPos + boardSize) % boardSize;
        if (forwardDistance >= 1 && forwardDistance <= 8) {
            candidates.push_back(other);
        }
    }

    if (candidates.empty()) {
        std::cout << "[INFO] Tidak ada pemain di depan dalam jangkauan 8 tile.\n\n";
        return;
    }

    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i + 1 << ". "
                  << candidates[i]->getUsername()
                  << " (posisi: " << candidates[i]->getPosition() << ")\n";
    }
    std::cout << "\n";

    std::cout << "Pilih pemain yang akan ditarik (nomor): ";
    int choice = 0;
    while (!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(candidates.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[INFO] Pilihan tidak valid. Masukkan nomor target yang benar: ";
    }

    Player* target = candidates[choice - 1];
    target->moveBackwardTo(currentPos, ctx);

    std::cout << "\n[INFO] " << target->getUsername() << " berhasil ditarik ke posisi: "
              << currentPos << "\n";
    
    // Bersihkan buffer input
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
