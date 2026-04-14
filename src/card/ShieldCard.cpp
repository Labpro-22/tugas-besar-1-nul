#include "ShieldCard.hpp"

ShieldCard::ShieldCard() : SkillCard("ShieldCard: Kebal tagihan/sanksi", 1) {}

void ShieldCard::use(Player* player, TurnContext& ctx)  {
    std::cout << "[MENGGUNAKAN] ShieldCard: Anda kebal tagihan selama giliran ini.\n";
    // TODO: Set flag isShieldActive = true pada objek Player
}


