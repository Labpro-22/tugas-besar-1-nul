#include "MoveCard.hpp"

MoveCard::MoveCard(int stp) : SkillCard("MoveCard: Maju " + std::to_string(stp) + " Petak", 0), steps(stp) {}

void MoveCard::use(Player* player, TurnContext& ctx) {
    std::cout << "[MENGGUNAKAN] MoveCard: Maju " << steps << " petak.\n";
    // TODO: Update posisi player sejauh 'steps' petak
    // TODO: Panggil fungsi pergerakan agar melewati GO tetap dapat gaji (jika lewat)
    }