#include "LassoCard.hpp"
#include "player/Player.h"

LassoCard::LassoCard() : SkillCard("LassoCard: Menarik pemain lawan", 0) {}

void LassoCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] LassoCard: Menarik pemain lawan ke posisi ini.\n";

    // TODO: Tampilkan daftar pemain lain yang posisinya berada di "depan"
    // TODO: Minta input pemain mana yang ditarik
    // TODO: Pindahkan pemain lawan tersebut ke petak player saat ini
    // TODO: Eksekusi onLanded untuk pemain lawan tersebut
}