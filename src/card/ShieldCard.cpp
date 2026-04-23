#include "card/ShieldCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"


void ShieldCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout << "[Skill] ShieldCard: You are immune to rent payment for the next roll\n";
    
    player.activateShield();
}
