#pragma once

#include <string>
#include <vector>

#include "property/Property.hpp"

class TurnContext;

struct LiquidationOption {
    enum Type { SELL, MORTGAGE };

    Property* property;
    Type type;
    int cashValue;

    LiquidationOption(Property* prop, Type t, int value)
        : property(prop), type(t), cashValue(value) {}
};

class BankruptcyManager {
  public:

    static int calculateMaxLiquidationFunds(TurnContext& ctx);
    static int calculateSellValue(TurnContext& ctx, Property* property);
    static bool canCoverDebt(TurnContext& ctx, int debtAmount);
    static std::vector<LiquidationOption> getAvailableLiquidationOptions(TurnContext& ctx);
    static void performForcedLiquidation(TurnContext& ctx, int debtAmount);
    static void declareBankrupt(TurnContext& ctx);
};
