#include "card/DemolitionCard.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "property/StreetProperty.hpp"

#include <iostream>
#include <limits>
#include <vector>

DemolitionCard::DemolitionCard()
    : SkillCard("DemolitionCard: Menghancurkan properti lawan", 0) {}

void DemolitionCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout
        << "[MENGGUNAKAN] DemolitionCard: Menghancurkan properti lawan.\n";

    std::vector<Player*> candidates;
    const std::vector<Player*> allPlayers = ctx.getAllPlayers();

    for (Player* other : allPlayers) {
        if (other == nullptr || other == &player)
            continue;
        candidates.push_back(other);
    }

    if (candidates.empty()) {
        std::cout << "[INFO] Tidak ada pemain lawan yang bisa ditarget.\n";
        return;
    }

    std::cout << "\n=== Daftar Pemain Lain ===\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i + 1 << ". " << candidates[i]->getUsername() << "\n";
    }

    std::cout << "\nPilih pemain yang propertinya akan dihancurkan (nomor): ";
    int playerChoice = 0;
    while (!(std::cin >> playerChoice) || playerChoice < 1 ||
           playerChoice > static_cast<int>(candidates.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout
            << "[INFO] Pilihan tidak valid. Masukkan nomor pemain yang benar: ";
    }

    Player* targetPlayer = candidates[playerChoice - 1];

    std::cout << "=== Properti yang Dapat Dihancurkan ===\n";

    const auto& properties = targetPlayer->getProperties();
    std::vector<size_t> demolishableIndices;

    for (size_t i = 0; i < properties.size(); ++i) {
        Property* prop = properties[i];

        StreetProperty* streetProp = dynamic_cast<StreetProperty*>(prop);
        if (streetProp != nullptr && streetProp->getBuildingCount() > 0) {
            demolishableIndices.push_back(i);
            std::string buildingType =
                streetProp->hasHotel() ? "Hotel" : "Rumah";
            std::cout << "[" << demolishableIndices.size() << "] "
                      << prop->getName() << " (" << buildingType << ")\n";
        }
    }

    if (demolishableIndices.empty()) {
        std::cout
            << "[INFO] Pemain " << targetPlayer->getUsername()
            << " tidak memiliki properti dengan bangunan untuk dihancurkan.\n";
        return;
    }

    std::cout << "\nPilih properti yang akan dihancurkan (nomor): ";
    int choice = 0;
    while (!(std::cin >> choice) || choice < 1 ||
           choice > static_cast<int>(demolishableIndices.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[INFO] Pilihan tidak valid. Masukkan nomor properti yang "
                     "benar: ";
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    size_t selectedIndex = demolishableIndices[choice - 1];
    StreetProperty* selectedProperty =
        dynamic_cast<StreetProperty*>(properties[selectedIndex]);

    if (selectedProperty != nullptr) {
        try {
            selectedProperty->demolish();
            std::cout << "\n[BERHASIL] Properti '"
                      << selectedProperty->getName()
                      << "' telah dihancurkan!\n";
            std::cout << "Tingkat bangunan sekarang: "
                      << selectedProperty->getBuildingCount() << "\n";
        } catch (const std::exception& e) {
            std::cout << "[GAGAL] " << e.what() << "\n";
        }
    }
}