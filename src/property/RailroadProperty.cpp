#include "property/RailroadProperty.hpp"

#include <utility>

#include "exception/InvalidGameStateException.hpp"

RailroadProperty::RailroadProperty(std::string code,
                                   std::string name,
                                   int buyPrice,
                                   int mortgageValue,
                                   std::map<int, int> rentTable)
    : Property(std::move(code), std::move(name), buyPrice, mortgageValue),
      rentTable_(std::move(rentTable)) {
    if (rentTable_.empty()) {
        throw InvalidGameStateException("Railroad rent table cannot be empty");
    }
}

int RailroadProperty::getRent(const TurnContext& ctx) const {
    (void) ctx;

    if (status_ != PropertyStatus::OWNED) {
        return 0;
    }
    if (owner_ == nullptr) {
        throw InvalidGameStateException(
            "Owned railroad property must have an owner");
    }
    if (!ownedRailroadCounter_) {
        throw InvalidGameStateException(
            "Railroad owned-count provider is not set");
    }

    const int ownedRailroads = ownedRailroadCounter_(owner_);
    return getRentFromOwnedCount(ownedRailroads);
}

int RailroadProperty::getRentFromOwnedCount(int ownedRailroads) const {
    if (ownedRailroads <= 0) {
        throw InvalidGameStateException(
            "Owned railroad count must be positive");
    }

    auto it = rentTable_.find(ownedRailroads);
    if (it == rentTable_.end()) {
        it = rentTable_.find(static_cast<int>(rentTable_.size()));
    }
    if (it == rentTable_.end()) {
        throw InvalidGameStateException(
            "No railroad rent mapping for owned count");
    }

    return it->second;
}

void RailroadProperty::setOwnedRailroadCounter(
    std::function<int(const Player*)> counter) {
    ownedRailroadCounter_ = std::move(counter);
}
