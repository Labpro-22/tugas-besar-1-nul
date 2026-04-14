#include "DiscountCard.hpp"

DiscountCard::DiscountCard(int discount) 
        : SkillCard("DiscountCard: Diskon " + std::to_string(discount) + "%", 1), 
          discountPercent(discount) {}

void DiscountCard::use(Player* player, TurnContext& ctx) {
    std::cout << "[MENGGUNAKAN] DiscountCard: Diskon " << discountPercent << "% aktif untuk 1 giliran.\n";
    // TODO: Set flag diskon pada objek Player
    // TODO: Simpan nilai 'discountPercent' di objek Player agar sewa bisa dipotong
}
