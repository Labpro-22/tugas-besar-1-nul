#include "card/CommunityChestCard.hpp"
#include <iostream>

CommunityChestCard::CommunityChestCard(std::string desc, CommunityChestType t, int amt) 
    : Card(std::move(desc)), type(t), amount(amt) {}

void CommunityChestCard::execute(Player* player, TurnContext& ctx) {
    std::cout << "[Dana Umum] " << description << "\n";
    
    switch (type) {
        case CommunityChestType::BIRTHDAY:
            std::cout << "Efek: Dapat M" << amount << " dari setiap pemain.\n";
            // Implementation: Collect from each player
            break;
        case CommunityChestType::DOCTOR_FEE:
            std::cout << "Efek: Bayar M" << amount << " untuk biaya dokter.\n";
            // Implementation: Pay to bank
            break;
        case CommunityChestType::POLITICAL_CAMPAIGN:
            std::cout << "Efek: Bayar M" << amount << " kepada setiap pemain.\n";
            // Implementation: Pay to each player
            break;
    }
}

CommunityChestType CommunityChestCard::getType() const {
    return type;
}

int CommunityChestCard::getAmount() const {
    return amount;
}