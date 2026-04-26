#include "card/SkillCard.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "exception/CardSlotFullException.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "gui/GuiPanelManager.hpp"

#include <iostream>
#include <limits>
#include "player/Player.hpp"

SkillCard::SkillCard(std::string desc, int duration)
    : Card(std::move(desc)), remainingDuration(duration) {}

int SkillCard::getDuration() const {
    return remainingDuration;
}

void SkillCard::decreaseDuration() {
    if (remainingDuration > 0)
        remainingDuration--;
}

void SkillCard::execute(Player* player, TurnContext& ctx) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot execute Skill Card: player is null");
    }
    
    if (player->getHandsAmount() >= 3) {
        // GUI mode: auto-discard first card to keep flow moving
        if (ctx.gameEngine.getPanelManager()) {
            try {
                player->discardSCard(0);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
                return;
            }
        } else {
            std::cout << "Hand is full! You have to discard 1 card.\n";
            
            player->showHands();
            
            std::cout << "\n";
            std::cout << "Choose a card to discard (1-3)\n";
            std::cout << "> ";
            int choice = 0;
            while (!(std::cin >> choice) || choice < 1 || choice > 3) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid Input! Please retry.\n\n";
                std::cout << "Choose a card to discard (1-3)\n";
                std::cout << "> ";
            }
            std::cout << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            try {
                player->discardSCard(choice - 1);
                std::cout << "Card discarded!\n";
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
                return;
            }
            
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    try {
        player->drawSCard(this);
    } catch (const CardSlotFullException& e) {
        std::cout << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }
}
