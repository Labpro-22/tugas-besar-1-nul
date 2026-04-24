#include "property/RailroadProperty.hpp"
#include "property/Property.hpp"
#include "player/Player.hpp"

#include <utility>
#include <iostream>

#include "exception/InvalidGameStateException.hpp"

// Creates railroad property with rent mapping by owned railroad count.
RailroadProperty::RailroadProperty(std::string code,
                                   std::string name,
                                   int buyPrice,
                                   int mortgageValue,
                                   std::map<int, int> rentTable,
                                    PropertyStatus status,
                                    int festivalMult,
                                    int festivalDur)
    : Property(std::move(code), std::move(name), buyPrice, mortgageValue, status, festivalMult, festivalDur),
      rentTable_(std::move(rentTable)) {
    if (rentTable_.empty()) {
        throw InvalidGameStateException("Railroad rent table cannot be empty");
    }
}

// Computes rent by querying owner railroad count through injected callback.
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

// Resolves railroad rent directly from number of owned railroads.
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

// Injects integration hook to count owned railroads for a given player.
void RailroadProperty::setOwnedRailroadCounter(
    std::function<int(const Player*)> counter) {
    ownedRailroadCounter_ = std::move(counter);
}

void RailroadProperty::printStatus(TurnContext& ctx){
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
    std::cout << "| Sewa saat ini    : M" << getRent(ctx) << "\n";
    std::cout << "+================================+\n";
}