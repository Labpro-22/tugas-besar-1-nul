#include "DiscountCard.hpp"
#include "player/Player.h"

DiscountCard::DiscountCard(int discount) 
        : SkillCard("DiscountCard: Diskon " + std::to_string(discount) + "%", 1), 
          discountPercent(discount) {}

void DiscountCard::apply(Player& player) {
    std::cout << "[MENGGUNAKAN] DiscountCard: Diskon " << discountPercent << "% aktif untuk 1 giliran.\n";
    player.setDiscountRate(discountPercent);
}
