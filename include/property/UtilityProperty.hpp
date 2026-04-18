#pragma once

#include <functional>
#include <map>
#include <string>

#include "property/Property.hpp"

class UtilityProperty : public Property {
  public:
    UtilityProperty(std::string code,
                    std::string name,
                    int buyPrice,
                    int mortgageValue,
                    std::map<int, int> multiplierTable);

    int getRent(const TurnContext& ctx) const override;

    int getRentFromOwnedCountAndDice(int ownedUtilities, int diceTotal) const;

    // Integration hooks to avoid depending on full Player/TurnContext
    // implementation.
    void setOwnedUtilityCounter(std::function<int(const Player*)> counter);
    void setDiceTotalExtractor(std::function<int(const TurnContext&)> extractor);
    void printStatus() override;

  private:
    std::map<int, int> multiplierTable_;
    std::function<int(const Player*)> ownedUtilityCounter_;
    std::function<int(const TurnContext&)> diceTotalExtractor_;
};
