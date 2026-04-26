#include "card/ShieldCard.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"

void ShieldCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[Skill] " << this->getDescription() << "\n";
    
    player.activateShield();
}
