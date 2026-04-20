#include "core/BankruptcyManager.hpp"

#include <algorithm>

#include "exception/BankruptcyException.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "player/Player.h"
#include "property/Property.hpp"
#include "property/StreetProperty.hpp"

int BankruptcyManager::calculateMaxLiquidationFunds(Player* player) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot calculate liquidation funds for null player");
    }

    int totalFunds = 0;

    for (const auto& property : player->getProperties()) {
        if (property == nullptr) continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            totalFunds += property->getMortgageValue();
        }

    }

    return totalFunds;
}

int BankruptcyManager::calculateSellValue(Property* property) {
    if (property == nullptr) {
        throw InvalidGameStateException("Cannot calculate sell value for null property");
    }

    int baseValue = property->getMortgageValue();

    StreetProperty* street = dynamic_cast<StreetProperty*>(property);
    if (street != nullptr && street->getBuildingCount() > 0) {
        int buildingCost = 0;

        if (street->hasHotel()) {
            buildingCost =
                (4 * street->getHousePrice()) + street->getHotelPrice();
        } else {
            buildingCost = street->getBuildingCount() * street->getHousePrice();
        }

        baseValue += buildingCost / 2;
    }

    return baseValue;
}

bool BankruptcyManager::canCoverDebt(Player* player, int debtAmount) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot check debt coverage for null player");
    }
    if (debtAmount <= 0) {
        throw InvalidGameStateException("Debt amount must be positive, got: " + std::to_string(debtAmount));
    }

    int currentBalance = player->getBalance();
    if (currentBalance >= debtAmount) {
        return true;
    }

    int remainingDebt = debtAmount - currentBalance;
    int liquidationFunds = calculateMaxLiquidationFunds(player);

    return liquidationFunds >= remainingDebt;
}

std::vector<LiquidationOption> BankruptcyManager::getAvailableLiquidationOptions(
    Player* player) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot get liquidation options for null player");
    }

    std::vector<LiquidationOption> options;

    for (auto property : player->getProperties()) {
        if (property == nullptr) continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            options.emplace_back(property, LiquidationOption::MORTGAGE,property->getMortgageValue());
            int sellValue = calculateSellValue(property);
            options.emplace_back(property, LiquidationOption::SELL, sellValue);
        }
    }

    std::sort(options.begin(), options.end(),[](const LiquidationOption& a, const LiquidationOption& b) {
        return a.cashValue > b.cashValue;
        });

    return options;
}

void BankruptcyManager::performForcedLiquidation(Player* player,int debtAmount) {
    if (player == nullptr) {
        throw BankruptcyException("Cannot liquidate null player");
    }

    int currentBalance = player->getBalance();
    if (currentBalance >= debtAmount) {
        return;  
    }

    int remainingDebt = debtAmount - currentBalance;
    auto options = getAvailableLiquidationOptions(player);

    std::stable_sort(options.begin(), options.end(),[](const LiquidationOption& a, const LiquidationOption& b) {
        if (a.type != b.type) {
            return a.type == LiquidationOption::MORTGAGE;
        }
        return a.cashValue > b.cashValue;
    });

    for (const auto& option : options) {
        if (remainingDebt <= 0) break;

        if (option.type == LiquidationOption::MORTGAGE) {
            option.property->mortgage();
            player->addCash(option.cashValue);
            remainingDebt -= option.cashValue;

        } 
        else {  
            int saleValue = option.property->sellToBank();
            player->addCash(saleValue);
            remainingDebt -= saleValue;
            // Remove property from player's list
            // Note: This is handled by sellToBank setting owner to nullptr
        }
    }

    if (remainingDebt > 0) {
        declareBankrupt(player);
        throw BankruptcyException(
            player->getUsername() +
            " has been declared bankrupt due to insufficient funds");
    }
}

void BankruptcyManager::declareBankrupt(Player* player) {
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot declare null player as bankrupt");
    }
    player->setBankruptStatus();
}
