#include "card/SkillCard.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "exception/CardSlotFullException.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "gui/GuiPanelManager.hpp"

#include <iostream>
#include <limits>
#include "player/Player.hpp"

SkillCard::SkillCard(std::string desc, int duration)
    : Card(std::move(desc)), remainingDuration(duration) {}

int SkillCard::getDuration() const {
    return remainingDuration;
}

void SkillCard::decreaseDuration() {
    if (remainingDuration > 0)
        remainingDuration--;
}

void SkillCard::execute(Player* player, TurnContext& ctx) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot execute SkillCard: player is null");
    }
    
    if (player->getHandsAmount() >= 3) {
        // GUI mode: auto-discard first card to keep flow moving
        if (ctx.gameEngine.getPanelManager()) {
            try {
                player->discardSCard(0);
            } catch (const std::exception& e) {
                std::cout << "[ERROR] Gagal membuang kartu: " << e.what() << "\n";
                return;
            }
        } else {
            std::cout << "[INFO] Tangan sudah penuh (3 kartu). Anda harus membuang 1 kartu.\n";
            
            player->showHands();
            
            std::cout << "\nPilih kartu yang akan dibuang (1-3): ";
            int choice = 0;
            while (!(std::cin >> choice) || choice < 1 || choice > 3) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "[INFO] Pilihan tidak valid. Masukkan nomor 1-3: ";
            }
            
            try {
                player->discardSCard(choice - 1);
                std::cout << "[INFO] Kartu berhasil dibuang.\n";
            } catch (const std::exception& e) {
                std::cout << "[ERROR] Gagal membuang kartu: " << e.what() << "\n";
                return;
            }
            
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    try {
        player->drawSCard(this);
    } catch (const CardSlotFullException& e) {
        std::cout << "[ERROR] " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cout << "[ERROR] Gagal menambahkan kartu: " << e.what() << "\n";
    }
}
