#include "card/DiscountCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"

DiscountCard::DiscountCard(int discount) 
        : SkillCard("DiscountCard: Diskon " + std::to_string(discount) + "%", 1), 
          discountPercent(discount) {}

void DiscountCard::apply(TurnContext& ctx) {
  Player& player = ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] DiscountCard: Diskon " << discountPercent << "% aktif untuk 1 giliran.\n";
    
    player.setDiscountRate(discountPercent);
}
