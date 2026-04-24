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
            std::cout << "Efek: Dapat M" << amount << " dari setiap pemain.\n";
            
            const std::vector<Player*>& allPlayers = ctx.getAllPlayers();
            int totalCollected = 0;
            
            for (Player* other : allPlayers) {
                if (other == nullptr || other == player) continue;
                if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
                
                try {
                    other->deductCash(amount);
                    totalCollected += amount;
                    std::cout << "[INFO] " << other->getUsername() << " membayar M" << amount << "\n";
                } catch (const std::exception& e) {
                    std::cout << "[INFO] " << other->getUsername() << " tidak mampu membayar M" << amount << "\n";
                }
            }
            
            if (totalCollected > 0) {
                player->addCash(totalCollected);
                std::cout << "[INFO] " << player->getUsername() << " menerima total M" << totalCollected << " dari pemain lain\n";
            } else {
                std::cout << "[INFO] Tidak ada pemain lain yang membayar.\n";
            }
            break;
        }
        
        case CommunityChestType::DOCTOR_FEE: {
            std::cout << "Efek: Bayar M" << amount << " untuk biaya dokter.\n";
            
            try {
                player->deductCash(amount);
                std::cout << "[INFO] " << player->getUsername() << " membayar M" << amount << " untuk biaya dokter.\n";
            } catch (const std::exception& e) {
                std::cout << "[INFO] " << player->getUsername() << " tidak memiliki cukup uang untuk biaya dokter.\n";
                player->checkBankruptcy(amount);
            }
            break;
        }
        
        case CommunityChestType::POLITICAL_CAMPAIGN: {
            std::cout << "Efek: Bayar M" << amount << " kepada setiap pemain.\n";
            
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
                std::cout << "[INFO] " << player->getUsername() << " tidak memiliki cukup uang (perlu M" << totalToPay << ").\n";
                player->checkBankruptcy(totalToPay);
                return;
            }
            
            for (Player* other : allPlayers) {
                if (other == nullptr || other == player) continue;
                if (other->getStatus() == PlayerStatus::BANKRUPT) continue;
                
                player->deductCash(amount);
                other->addCash(amount);
                std::cout << "[INFO] " << player->getUsername() << " membayar M" << amount << " ke " << other->getUsername() << "\n";
            }
            
            std::cout << "[INFO] Total dibayar: M" << totalToPay << " kepada " << validRecipients << " pemain.\n";
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
