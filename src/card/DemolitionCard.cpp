#include "card/DemolitionCard.hpp"
#include "player/Player.h"
#include "property/StreetProperty.hpp"
#include <iostream>
#include <limits>

DemolitionCard::DemolitionCard() : SkillCard("DemolitionCard: Menghancurkan properti lawan", 0) {}

void DemolitionCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] DemolitionCard: Menghancurkan properti lawan.\n";
    

    
    std::cout << "\n=== Daftar Pemain Lain ===\n";
    std::cout << "Pemain: " << player.getUsername() << " (saat ini)\n";
    std::cout << "TODO: Tampilkan daftar pemain lain\n\n";
    
    // Untuk demonstrasi
    std::cout << "=== Properti yang Dapat Dihancurkan ===\n";
    
    const auto& properties = player.getProperties();
    std::vector<size_t> demolishableIndices;
    
    for (size_t i = 0; i < properties.size(); ++i) {
        Property* prop = properties[i];
        
        StreetProperty* streetProp = dynamic_cast<StreetProperty*>(prop);
        if (streetProp != nullptr && streetProp->getBuildingCount() > 0) {
            demolishableIndices.push_back(i);
            std::string buildingType = streetProp->hasHotel() ? "Hotel" : "Rumah";
            std::cout << "[" << demolishableIndices.size() << "] " 
                      << prop->getName() << " (" << buildingType << ")\n";
        }
    }
    
    if (demolishableIndices.empty()) {
        std::cout << "Pemain tidak memiliki properti dengan bangunan untuk dihancurkan.\n";
        return;
    }
    
    std::cout << "\nPilih properti yang akan dihancurkan (nomor): ";
    int choice;
    std::cin >> choice;
    
    if (choice < 1 || choice > static_cast<int>(demolishableIndices.size())) {
        std::cout << "Pilihan tidak valid.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    size_t selectedIndex = demolishableIndices[choice - 1];
    StreetProperty* selectedProperty = dynamic_cast<StreetProperty*>(properties[selectedIndex]);
    
    if (selectedProperty != nullptr) {
        try {
            selectedProperty->demolish();
            std::cout << "\n[BERHASIL] Properti '" << selectedProperty->getName() << "' telah dihancurkan!\n";
            std::cout << "Tingkat bangunan sekarang: " << selectedProperty->getBuildingCount() << "\n";
        } catch (const std::exception& e) {
            std::cout << "[GAGAL] " << e.what() << "\n";
        }
    }
}