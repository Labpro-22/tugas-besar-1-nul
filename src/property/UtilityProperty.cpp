#include "property/UtilityProperty.hpp"

#include <utility>

#include "exception/InvalidGameStateException.hpp"

UtilityProperty::UtilityProperty(std::string code,
                                 std::string name,
                                 int buyPrice,
                                 int mortgageValue,
                                 std::map<int, int> multiplierTable)
    : Property(std::move(code), std::move(name), buyPrice, mortgageValue),
      multiplierTable_(std::move(multiplierTable)) {
    if (multiplierTable_.empty()) {
        throw InvalidGameStateException(
            "Utility multiplier table cannot be empty");
    }
}

int UtilityProperty::getRent(const TurnContext& ctx) const {
    if (status_ != PropertyStatus::OWNED) {
        return 0;
    }
    if (owner_ == nullptr) {
        throw InvalidGameStateException(
            "Owned utility property must have an owner");
    }
    if (!ownedUtilityCounter_) {
        throw InvalidGameStateException(
            "Utility owned-count provider is not set");
    }
    if (!diceTotalExtractor_) {
        throw InvalidGameStateException(
            "Utility dice-total provider is not set");
    }

    const int ownedUtilities = ownedUtilityCounter_(owner_);
    const int diceTotal = diceTotalExtractor_(ctx);
    return getRentFromOwnedCountAndDice(ownedUtilities, diceTotal);
}

int UtilityProperty::getRentFromOwnedCountAndDice(int ownedUtilities,
                                                  int diceTotal) const {
    if (ownedUtilities <= 0) {
        throw InvalidGameStateException("Owned utility count must be positive");
    }
    if (diceTotal <= 0) {
        throw InvalidGameStateException(
            "Dice total must be positive for utility rent");
    }

    auto it = multiplierTable_.find(ownedUtilities);
    if (it == multiplierTable_.end()) {
        it = multiplierTable_.find(static_cast<int>(multiplierTable_.size()));
    }
    if (it == multiplierTable_.end()) {
        throw InvalidGameStateException(
            "No utility multiplier mapping for owned count");
    }

    return it->second * diceTotal;
}

void UtilityProperty::setOwnedUtilityCounter(
    std::function<int(const Player*)> counter) {
    ownedUtilityCounter_ = std::move(counter);
}

void UtilityProperty::setDiceTotalExtractor(
    std::function<int(const TurnContext&)> extractor) {
    diceTotalExtractor_ = std::move(extractor);
}
