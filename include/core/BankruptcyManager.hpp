#pragma once

#include <string>
#include <vector>

#include "property/Property.hpp"

class Player;

struct LiquidationOption {
    enum Type { SELL, MORTGAGE };

    Property* property;
    Type type;
    int cashValue;  

    LiquidationOption(Property* prop, Type t, int value): property(prop), type(t), cashValue(value) {}
};

class BankruptcyManager {
  public:

    static int calculateMaxLiquidationFunds(Player* player);
    static int calculateSellValue(Property* property);
    static bool canCoverDebt(Player* player, int debtAmount);
    static std::vector<LiquidationOption> getAvailableLiquidationOptions(Player* player);
    static void performForcedLiquidation(Player* player, int debtAmount);
    static void declareBankrupt(Player* player);
};
