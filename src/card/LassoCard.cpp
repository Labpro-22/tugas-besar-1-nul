#include "card/LassoCard.hpp"
#include "board/Board.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "gui/GuiPanelManager.hpp"
#include "exception/InvalidGameStateException.hpp"

#include <iostream>
#include <limits>
#include <vector>

LassoCard::LassoCard() : SkillCard("|Lasso Card| : Pull any enemies within 8 tiles", 0) {}

void LassoCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    const int currentPos = player.getPosition();
    const int boardSize = ctx.getBoardSize();

    if (boardSize <= 0) {
        throw InvalidGameStateException("Lasso Card: Invalid board size: " + std::to_string(boardSize));
    }

    std::cout << "[SKILL] " << this->getDescription() << "\n";

    std::cout << "\n=== List of Players Ahead ===\n";
    std::cout << "Current player: " << player.getUsername()
              << " (position: " << currentPos << ")\n";

    std::vector<Player*> candidates;
    const std::vector<Player*> allPlayers = ctx.getAllPlayers();
    for (Player* other : allPlayers) {
        if (other == nullptr || other == &player)
            continue;

        const int forwardDistance =
            (other->getPosition() - currentPos + boardSize) % boardSize;
        if (forwardDistance >= 1 && forwardDistance <= 8) {
            candidates.push_back(other);
        }
    }

    if (candidates.empty()) {
        std::cout
            << "[SKILL] No player found within 8 tiles.\n\n";
        return;
    }

    // GUI mode: auto-select first candidate
    if (ctx.gameEngine.getPanelManager()) {
        Player* target = candidates.front();
        target->moveBackwardTo(currentPos, ctx);
        std::cout << "\n[SKILL] " << target->getUsername()
                  << " successfully pulled to: " << currentPos << "\n";
        return;
    }

    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i + 1 << ". " << candidates[i]->getUsername()
                  << " (position: " << candidates[i]->getPosition() << ")\n";
    }
    std::cout << "\n";

    int choice = 0;
    if (candidates.size() == 1) {
        choice = 1;
    }
    std::cout << "Choose a player to pull (1-" << candidates.size() << ")\n";
    std::cout << "> ";
    while (!(std::cin >> choice) || choice < 1 ||
           choice > static_cast<int>(candidates.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input! Please retry\n\n";
        std::cout << "Choose a player to pull (1-" << candidates.size() << ")\n";
        std::cout << "> ";
    }
    std::cout << "\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Player* target = candidates[choice - 1];
    target->moveBackwardTo(currentPos, ctx);

    std::cout << "[" << target->getUsername()
              << "] successfully pulled to: " << currentPos << "\n\n";
}
