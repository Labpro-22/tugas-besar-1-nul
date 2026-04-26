#include "card/DemolitionCard.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"
#include "property/StreetProperty.hpp"
#include "gui/GuiPanelManager.hpp"
#include "exception/InvalidGameStateException.hpp"

#include <iostream>
#include <limits>
#include <vector>

DemolitionCard::DemolitionCard()
    : SkillCard("|Demolition Card|: Destroy any enemy property", 0) {}

void DemolitionCard::apply(TurnContext& ctx) {
    Player& player = ctx.currentPlayer;
    std::cout
        << "[SKILL] " << this->getDescription() << "\n";

    std::vector<Player*> candidates;
    const std::vector<Player*> allPlayers = ctx.getAllPlayers();

    for (Player* other : allPlayers) {
        if (other == nullptr || other == &player) continue;
        if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
        candidates.push_back(other);
    }

    if (candidates.empty()) {
        std::cout << "No players are targetted.\n";
        return;
    }

    // GUI mode: auto-select first candidate
    if (ctx.gameEngine.getPanelManager()) {
        Player* targetPlayer = candidates.front();
        const auto& properties = targetPlayer->getProperties();
        for (Property* prop : properties) {
            StreetProperty* sp = dynamic_cast<StreetProperty*>(prop);
            if (sp != nullptr && sp->getBuildingCount() > 0) {
                try {
                    sp->demolish();
                    std::cout << "Property '" << sp->getName()
                              << "' destroyed!\n";
                } catch (const std::exception& e) {
                    std::cout << e.what() << "\n";
                }
                return;
            }
        }
        std::cout << "[" << targetPlayer->getUsername()
                  << "] has no property to be destroyed.\n";
        return;
    }

    std::cout << "\n=== List of Other Players ===\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i + 1 << ". " << candidates[i]->getUsername() << "\n";
    }

    std::cout << "\n";
    std::cout << "Choose a player whose property you will destroy (number)\n";
    std::cout << "> ";
    int playerChoice = 0;
    while (!(std::cin >> playerChoice) || playerChoice < 1 ||
           playerChoice > static_cast<int>(candidates.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input! Please retry\n\n";
        std::cout << "Choose a player whose property you will destroy (number)\n";
        std::cout << "> ";
    }
    std::cout << "\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Player* targetPlayer = candidates[playerChoice - 1];

    std::cout << "=== Properti yang Dapat Dihancurkan ===\n";

    const auto& properties = targetPlayer->getProperties();
    std::vector<size_t> demolishableIndices;

    // GUI mode: auto-select first candidate and first demolishable property
    if (ctx.gameEngine.getPanelManager()) {
        for (size_t i = 0; i < properties.size(); ++i) {
            StreetProperty* sp = dynamic_cast<StreetProperty*>(properties[i]);
            if (sp != nullptr && sp->getBuildingCount() > 0) {
                try {
                    sp->demolish();
                    std::cout << "Property '" << sp->getName()
                              << "' destroyed!\n";
                } catch (const std::exception& e) {
                    std::cout << e.what() << "\n";
                }
                return;
            }
        }
        std::cout << "[" << targetPlayer->getUsername()
                  << "] has no property to be destroyed.\n";
        return;
    }

    for (size_t i = 0; i < properties.size(); ++i) {
        Property* prop = properties[i];

        StreetProperty* streetProp = dynamic_cast<StreetProperty*>(prop);
        if (streetProp != nullptr && streetProp->getBuildingCount() > 0) {
            demolishableIndices.push_back(i);
            std::string buildingType =
                streetProp->hasHotel() ? "Hotel" : "House";
            std::cout << "[" << demolishableIndices.size() << "] "
                      << prop->getName() << " (" << buildingType << ")\n";
        }
    }

    if (demolishableIndices.empty()) {
        std::cout
            << "[" << targetPlayer->getUsername()
            << "] has no property to be destroyed.\n";
        return;
    }

    std::cout << "\n";
    std::cout << "Choose the property you want to destroy (number)\n";
    std::cout << "> ";
    int choice = 0;
    while (!(std::cin >> choice) || choice < 1 ||
           choice > static_cast<int>(demolishableIndices.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input! Please retry\n\n";
        std::cout << "Choose the property you want to destroy (number)\n";
        std::cout << "> ";
    }
    std::cout << "\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    size_t selectedIndex = demolishableIndices[choice - 1];
    StreetProperty* selectedProperty =
        dynamic_cast<StreetProperty*>(properties[selectedIndex]);

    if (selectedProperty != nullptr) {
        try {
            selectedProperty->demolish();
            std::cout << "\nProperty '"
                      << selectedProperty->getName()
                      << "' destroyed!\n";
            std::cout << "Current housing level: "
                      << selectedProperty->getBuildingCount() << "\n";
        } catch (const std::exception& e) {
            std::cout << e.what() << "\n";
        }
    }
}