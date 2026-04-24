#include "card/DiscountCard.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "exception/InvalidGameStateException.hpp"

DiscountCard::DiscountCard(int discount) 
        : SkillCard("DiscountCard: Diskon " + std::to_string(discount) + "%", 1), 
          discountPercent(discount) {
    if (discount < 0 || discount > 100) {
        throw InvalidGameStateException("DiscountCard discount must be between 0 and 100, got: " + std::to_string(discount));
    }
}

void DiscountCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[MENGGUNAKAN] DiscountCard: Diskon " << discountPercent
              << "% aktif untuk 1 giliran.\n";

    player.setDiscountRate(discountPercent);
}
