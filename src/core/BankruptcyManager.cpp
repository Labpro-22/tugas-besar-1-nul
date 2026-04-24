#include "core/BankruptcyManager.hpp"

#include <algorithm>

#include "core/TurnContext.hpp"
#include "exception/BankruptcyException.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"
#include "property/StreetProperty.hpp"

int BankruptcyManager::calculateMaxLiquidationFunds(TurnContext& ctx) {
    Player* player = &ctx.currentPlayer;

    int totalFunds = 0;

    for (const auto& property : player->getProperties()) {
        if (property == nullptr) continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            totalFunds += property->getMortgageValue();
        }

    }

    return totalFunds;
}

int BankruptcyManager::calculateSellValue(TurnContext& ctx, Property* property) {
    (void)ctx;
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

bool BankruptcyManager::canCoverDebt(TurnContext& ctx, int debtAmount) {
    Player* player = &ctx.currentPlayer;
    if (debtAmount <= 0) {
        throw InvalidGameStateException("Debt amount must be positive, got: " + std::to_string(debtAmount));
    }

    int currentBalance = player->getBalance();
    if (currentBalance >= debtAmount) {
        return true;
    }

    int remainingDebt = debtAmount - currentBalance;
    int liquidationFunds = calculateMaxLiquidationFunds(ctx);

    return liquidationFunds >= remainingDebt;
}

std::vector<LiquidationOption> BankruptcyManager::getAvailableLiquidationOptions(
    TurnContext& ctx) {
    Player* player = &ctx.currentPlayer;

    std::vector<LiquidationOption> options;

    for (auto property : player->getProperties()) {
        if (property == nullptr) continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            options.emplace_back(property, LiquidationOption::MORTGAGE,property->getMortgageValue());
            int sellValue = calculateSellValue(ctx, property);
            options.emplace_back(property, LiquidationOption::SELL, sellValue);
        }
    }

    std::sort(options.begin(), options.end(),[](const LiquidationOption& a, const LiquidationOption& b) {
        return a.cashValue > b.cashValue;
        });

    return options;
}

void BankruptcyManager::performForcedLiquidation(TurnContext& ctx,int debtAmount) {
    Player* player = &ctx.currentPlayer;

    int currentBalance = player->getBalance();
    if (currentBalance >= debtAmount) {
        return;  
    }

    int remainingDebt = debtAmount - currentBalance;
    auto options = getAvailableLiquidationOptions(ctx);


    std::stable_sort(options.begin(), options.end(),[](const LiquidationOption& a, const LiquidationOption& b) {
        if (a.type != b.type) {
            return a.type == LiquidationOption::MORTGAGE;
        }
        return a.cashValue > b.cashValue;
    });

    for (const auto& option : options) {
        if (remainingDebt <= 0) break;
        if (option.property == nullptr || option.property->getStatus() != PropertyStatus::OWNED) {
            continue;
        }

        if (option.type == LiquidationOption::MORTGAGE) {
            player->mortgage(option.property);
            remainingDebt -= option.property->getMortgageValue();

        } 
        else {  
            int saleValue = option.property->sellToBank();
            player->addCash(saleValue);
            remainingDebt -= saleValue;
            player->removeProperty(*option.property);
        }
    }

    if (remainingDebt > 0) {
        declareBankrupt(ctx);
        throw BankruptcyException(
            player->getUsername() +
            " has been declared bankrupt due to insufficient funds");
    }
}

void BankruptcyManager::declareBankrupt(TurnContext& ctx) {
    Player* player = &ctx.currentPlayer;
    player->setBankruptStatus();
}
