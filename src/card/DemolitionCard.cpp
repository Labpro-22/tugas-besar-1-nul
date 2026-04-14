#include "DemolitionCard.hpp"

DemolitionCard::DemolitionCard() : SkillCard("DemolitionCard: Menghancurkan properti lawan", 0) {}

void DemolitionCard::use(Player* player, TurnContext& ctx)  {
    std::cout << "[MENGGUNAKAN] DemolitionCard: Menghancurkan properti lawan.\n";
    // TODO: Tampilkan daftar pemain lain
    // TODO: Tampilkan properti milik target yang memiliki bangunan (Rumah/Hotel)
    // TODO: Minta input properti mana yang akan dihancurkan
    // TODO: Turunkan level bangunan (Hotel -> 4 Rumah, atau Rumah--).
}