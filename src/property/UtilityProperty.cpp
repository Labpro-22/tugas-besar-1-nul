#include "property/UtilityProperty.hpp"
#include "property/Property.hpp"
#include "player/Player.hpp"
#include "core/Dice.hpp"
#include "core/TurnContext.hpp"

#include <iostream>
#include <utility>

#include "exception/InvalidGameStateException.hpp"

// Creates utility property with multiplier mapping by owned utility count.
UtilityProperty::UtilityProperty(std::string code,
                                 std::string name,
                                 int buyPrice,
                                 int mortgageValue,
                                 std::map<int, int> multiplierTable,
                                 PropertyStatus status,
                                 int festivalMult,
                                 int festivalDur)
    : Property(std::move(code),
               std::move(name),
               buyPrice,
               mortgageValue,
               status,
               festivalMult,
               festivalDur),
      multiplierTable_(std::move(multiplierTable)) {
    if (multiplierTable_.empty()) {
        throw InvalidGameStateException(
            "Utility multiplier table cannot be empty");
    }
}

// Computes utility rent from owned count and dice total callbacks.
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

    const int ownedUtilities = ownedUtilityCounter_(owner_);

    const int diceTotal = diceTotalExtractor_ ? diceTotalExtractor_(ctx)
                                              : ctx.dice.getTotal();
    return getRentFromOwnedCountAndDice(ownedUtilities, diceTotal);
}

// Resolves utility rent directly from owned utility count and dice total.
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

// Injects integration hook to count owned utilities for a given player.
void UtilityProperty::setOwnedUtilityCounter(
    std::function<int(const Player*)> counter) {
    ownedUtilityCounter_ = std::move(counter);
}

// Injects integration hook to read dice total from turn context.
void UtilityProperty::setDiceTotalExtractor(
    std::function<int(const TurnContext&)> extractor) {
    diceTotalExtractor_ = std::move(extractor);
}

void UtilityProperty::printStatus(TurnContext& ctx) {
    std::cout << "+================================+\n";
    std::cout << "| [" <<  "] " << getName() << " (" << getCode() << ")\n";
    std::string stat = "BANK";
    if (getStatus() == PropertyStatus::OWNED){
        stat = "OWNED BY [" + getOwner()->getUsername() + "]";
    } else if (getStatus() == PropertyStatus::MORTGAGED){
        stat = "MORTGAGED BY [" + getOwner()->getUsername() + "]";
    }
    std::cout << "| Status: " << stat << "\n";
    std::cout << "| Harga Beli    : M" << getBuyPrice() << "\n";
    std::string faktorPengaliText = "-";
    if (getStatus() == PropertyStatus::OWNED && getOwner() != nullptr && ownedUtilityCounter_) {
        const int ownedUtilities = ownedUtilityCounter_(getOwner());
        auto it = multiplierTable_.find(ownedUtilities);
        if (it == multiplierTable_.end()) {
            it = multiplierTable_.find(static_cast<int>(multiplierTable_.size()));
        }
        if (it != multiplierTable_.end()) {
            faktorPengaliText = std::to_string(it->second);
        }
    }

    std::cout << "| Sewa saat ini    : <banyak dadu ketika mendarat> x M"
              << faktorPengaliText << "\n";
    std::cout << "+================================+\n";
}