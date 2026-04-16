#include "DemolitionCard.hpp"
#include "player/Player.h"

DemolitionCard::DemolitionCard() : SkillCard("DemolitionCard: Menghancurkan properti lawan", 0) {}

void DemolitionCard::apply(Player& player)  {
    std::cout << "[MENGGUNAKAN] DemolitionCard: Menghancurkan properti lawan.\n";
    // TODO: Tampilkan daftar pemain lain
    // TODO: Tampilkan properti milik target yang memiliki bangunan (Rumah/Hotel)
    // TODO: Minta input properti mana yang akan dihancurkan
    // TODO: Turunkan level bangunan (Hotel -> 4 Rumah, atau Rumah--).
}