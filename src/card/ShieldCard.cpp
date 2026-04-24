#include "card/ShieldCard.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"

void ShieldCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[Skill] ShieldCard: You are immune to rent payment for the next roll\n";
    
    player.activateShield();
}
