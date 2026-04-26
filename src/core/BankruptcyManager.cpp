#include "core/BankruptcyManager.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

#include "core/TurnContext.hpp"
#include "exception/BankruptcyException.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"
#include "property/RailroadProperty.hpp"
#include "property/StreetProperty.hpp"
#include "property/UtilityProperty.hpp"

using namespace std;

int BankruptcyManager::calculateMaxLiquidationFunds(TurnContext& ctx) {
    Player* player = &ctx.currentPlayer;

    int totalFunds = 0;

    for (const auto& property : player->getProperties()) {
        if (property == nullptr)
            continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            totalFunds += property->getMortgageValue();
        }
    }

    return totalFunds;
}

int BankruptcyManager::calculateSellValue(TurnContext& ctx, Property* property) {
    (void)ctx;
    if (property == nullptr) {
        throw InvalidGameStateException(
            "Cannot calculate sell value for null property");
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
        throw InvalidGameStateException("Debt amount must be positive, got: " +
                                        std::to_string(debtAmount));
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
        if (property == nullptr)
            continue;

        if (property->getStatus() == PropertyStatus::OWNED) {
            options.emplace_back(property, LiquidationOption::MORTGAGE,property->getMortgageValue());
            int sellValue = calculateSellValue(ctx, property);
            options.emplace_back(property, LiquidationOption::SELL, sellValue);
        }
    }

    std::sort(options.begin(),
              options.end(),
              [](const LiquidationOption& a, const LiquidationOption& b) {
                  return a.cashValue > b.cashValue;
              });

    return options;
}

bool BankruptcyManager::resolveDebtByLiquidation(TurnContext& ctx, int debtAmount) {
    Player* player = &ctx.currentPlayer;

    while (player->getBalance() < debtAmount) {
        auto options = getAvailableLiquidationOptions(ctx);
        if (options.empty()) {
            declareBankrupt(ctx);
            cout << player->getUsername() << " cannot cover rent and is bankrupt\n";
            return false;
        }

        cout << "=== Panel Likuidasi ===\n";
        cout << "Uang kamu saat ini: M" << player->getBalance()
             << "  |  Kewajiban: M" << debtAmount << "\n\n";

        vector<LiquidationOption> sellOptions;
        vector<LiquidationOption> mortgageOptions;
        for (const auto& option : options) {
            if (option.property == nullptr || option.property->getStatus() != PropertyStatus::OWNED) {
                continue;
            }

            if (option.type == LiquidationOption::SELL) {
                sellOptions.push_back(option);
            } else {
                mortgageOptions.push_back(option);
            }
        }

        vector<LiquidationOption> numberedOptions;
        int menuIndex = 1;

        cout << "[Jual ke Bank]\n";
        if (sellOptions.empty()) {
            cout << "- Tidak ada properti yang bisa dijual\n";
        } else {
            for (const auto& option : sellOptions) {
                Property* prop = option.property;
                string typeLabel = "LAIN";
                string extraLabel;

                if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
                    typeLabel = street->getColorGroup();
                    if (street->hasHotel()) {
                        const int housePart = 4 * street->getHousePrice();
                        const int hotelPart = street->getHotelPrice();
                        extraLabel = " (termasuk hotel: M" + to_string(housePart + hotelPart) + ")";
                    } else if (street->getBuildingCount() > 0) {
                        const int buildingPart = street->getBuildingCount() * street->getHousePrice();
                        extraLabel = " (termasuk " + to_string(street->getBuildingCount()) +
                                     " rumah: M" + to_string(buildingPart / 2) + ")";
                    }
                } else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
                    typeLabel = "STATION";
                } else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
                    typeLabel = "UTILITY";
                }

                cout << menuIndex << ". " << prop->getName() << " (" << prop->getCode() << ")"
                     << "  [" << typeLabel << "]"
                     << "  Harga Jual: M" << option.cashValue
                     << extraLabel << "\n";
                numberedOptions.push_back(option);
                ++menuIndex;
            }
        }

        cout << "\n[Gadaikan]\n";
        if (mortgageOptions.empty()) {
            cout << "- Tidak ada properti yang bisa digadaikan\n";
        } else {
            for (const auto& option : mortgageOptions) {
                Property* prop = option.property;
                string typeLabel = "LAIN";
                if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
                    typeLabel = street->getColorGroup();
                } else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
                    typeLabel = "STATION";
                } else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
                    typeLabel = "UTILITY";
                }

                cout << menuIndex << ". " << prop->getName() << " (" << prop->getCode() << ")"
                     << "  [" << typeLabel << "]"
                     << "  Nilai Gadai: M" << option.cashValue << "\n";
                numberedOptions.push_back(option);
                ++menuIndex;
            }
        }

        if (numberedOptions.empty()) {
            declareBankrupt(ctx);
            cout << player->getUsername() << " cannot cover rent and is bankrupt\n";
            return false;
        }

        int selected = 0;
        cout << "\nPilih aksi likuidasi (nomor): ";
        while (!(cin >> selected) || selected < 1 || selected > static_cast<int>(numberedOptions.size())) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid, masukkan nomor yang benar: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        const LiquidationOption choice = numberedOptions[static_cast<size_t>(selected - 1)];
        if (choice.property == nullptr || choice.property->getStatus() != PropertyStatus::OWNED) {
            cout << "Properti tidak valid untuk diproses.\n\n";
            continue;
        }

        try {
            if (choice.type == LiquidationOption::MORTGAGE) {
                player->mortgage(choice.property);
                cout << "[LIKUIDASI] Berhasil menggadaikan " << choice.property->getName()
                     << " untuk M" << choice.cashValue << "\n\n";
            } else {
                const int saleValue = choice.property->sellToBank();
                player->addCash(saleValue);
                player->sell(*choice.property);
                cout << "[LIKUIDASI] Berhasil menjual " << choice.property->getName()
                     << " ke bank untuk M" << saleValue << "\n\n";
            }
        } catch (const std::exception& ex) {
            cout << "[LIKUIDASI] Gagal: " << ex.what() << "\n\n";
        }
    }

    return true;
}

void BankruptcyManager::performForcedLiquidation(TurnContext& ctx,int debtAmount) {
    Player* player = &ctx.currentPlayer;

    int currentBalance = player->getBalance();
    if (currentBalance >= debtAmount) {
        return;
    }

    int remainingDebt = debtAmount - currentBalance;
    auto options = getAvailableLiquidationOptions(ctx);

    std::stable_sort(
        options.begin(),
        options.end(),
        [](const LiquidationOption& a, const LiquidationOption& b) {
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

        } else {
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
