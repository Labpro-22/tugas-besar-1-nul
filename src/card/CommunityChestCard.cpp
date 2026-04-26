#include "card/CommunityChestCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "exception/InvalidGameStateException.hpp"

#include <iostream>

CommunityChestCard::CommunityChestCard(std::string desc,
                                       CommunityChestType t,
                                       int amt)
    : Card(std::move(desc)), type(t), amount(amt) {}

void CommunityChestCard::execute(Player* player, TurnContext& ctx) {
    std::cout << "[Dana Umum] " << description << "\n";
    
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot execute CommunityChestCard: player is null");
    }
    
    switch (type) {
        case CommunityChestType::BIRTHDAY: {
            std::cout << "Effect: Collect M" << amount << " from every players.\n";
            
            const std::vector<Player*>& allPlayers = ctx.getAllPlayers();
            int totalCollected = 0;
            
            for (Player* other : allPlayers) {
                if (other == nullptr || other == player) continue;
                if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
                
                try {
                    other->deductCash(amount);
                    totalCollected += amount;
                    std::cout << "[" << other->getUsername() << "] paid M" << amount << "\n";
                } catch (const std::exception& e) {
                    std::cout << "[" << other->getUsername() << "] couldn't pay " << amount << "\n";
                }
            }
            
            if (totalCollected > 0) {
                player->addCash(totalCollected);
                std::cout << "[" << player->getUsername() << "] received a total of M" << totalCollected << " from other players\n";
            } else {
                std::cout << "No other players paid.\n";
            }
            break;
        }
        
        case CommunityChestType::DOCTOR_FEE: {
            std::cout << "Effect: Pay M" << amount << " after your doctor visit.\n";

            if (player->isShieldActive()) {
                std::cout << "[" << player->getUsername() << "] is protected by a shield and does not have to pay the doctor fee.\n\n";
                player->resetTurnSkills();
                return;
            }
            
            if (player->getDiscountRate() > 0) {
                int discountedAmount = amount * (100 - player->getDiscountRate()) / 100;
                std::cout << "Applying discount: M" << amount << " -> M" << discountedAmount << "\n";
                amount = discountedAmount;
                player->resetTurnSkills();
            }

            try {
                player->deductCash(amount);
                std::cout << "[" << player->getUsername() << " paid M" << amount << " for doctor fee.\n";
            } catch (const std::exception& e) {
                std::cout << "[" << player->getUsername() << " doesn't have enough money to pay the doctor fee.\n";
                player->checkBankruptcy(amount);
            }
            break;
        }
        
        case CommunityChestType::POLITICAL_CAMPAIGN: {
            std::cout << "Effect: Pay M" << amount << " to every other players as a political campaign.\n";
            
            if (player->isShieldActive()) {
                std::cout << "[" << player->getUsername() << "] is protected by a shield and does not have to pay for the political campaign.\n\n";
                player->resetTurnSkills();
                return;
            }

            if (player->getDiscountRate() > 0) {
                int discountedAmount = amount * (100 - player->getDiscountRate()) / 100;
                std::cout << "Applying discount: M" << amount << " -> M" << discountedAmount << "\n";
                amount = discountedAmount;
                player->resetTurnSkills();
            }

            const std::vector<Player*>& allPlayers = ctx.getAllPlayers();
            int totalToPay = 0;
            int validRecipients = 0;
            
            for (Player* other : allPlayers) {
                if (other == nullptr || other == player) continue;
                if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
                totalToPay += amount;
                validRecipients++;
            }
            
            if (player->getBalance() < totalToPay) {
                std::cout << "[" << player->getUsername() << "] doesn't have enough money to pay (requires M" << totalToPay << ").\n";
                player->checkBankruptcy(totalToPay);
                return;
            }
            
            for (Player* other : allPlayers) {
                if (other == nullptr || other == player) continue;
                if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
                
                player->deductCash(amount);
                other->addCash(amount);
                std::cout << "[" << player->getUsername() << " paid M" << amount << " to " << other->getUsername() << "\n";
            }
            
            std::cout << "Total paid: M" << totalToPay << " to " << validRecipients << " players.\n";
            break;
        }
    }
}

CommunityChestType CommunityChestCard::getType() const {
    return type;
}

int CommunityChestCard::getAmount() const {
    return amount;
}
