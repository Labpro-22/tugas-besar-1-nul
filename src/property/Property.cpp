// #include "property/Property.hpp"
#include "../../include/property/Property.hpp"
#include "player/Player.hpp"
#include "property/StreetProperty.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"

#include <iostream>
#include <utility>

#include "../../include/exception/InvalidGameStateException.hpp"

namespace {
void recomputeStreetMonopolyForColor(TurnContext& ctx, const std::string& targetColor) {
    std::vector<StreetProperty*> sameColorStreets;
    sameColorStreets.reserve(4);

    for (Property* p : ctx.board.getAllProperties()) {
        StreetProperty* sp = dynamic_cast<StreetProperty*>(p);
        if (sp != nullptr && sp->getColor() == targetColor) {
            sameColorStreets.push_back(sp);
        }
    }

    if (sameColorStreets.empty()) {
        return;
    }

    Player* candidateOwner = sameColorStreets.front()->getOwner();
    bool monopolized = candidateOwner != nullptr;

    for (StreetProperty* sp : sameColorStreets) {
        if (sp->getOwner() != candidateOwner) {
            monopolized = false;
            break;
        }
    }

    for (StreetProperty* sp : sameColorStreets) {
        sp->setMonopolized(monopolized);
    }
}
}

// Initializes common property state and validates basic invariants.
Property::Property(std::string code,
                   std::string name,
                   int buyPrice,
                   int mortgageValue,
                   PropertyStatus status,
                   int festivalMult,
                   int festivalDur)
    : code_(std::move(code)), name_(std::move(name)), status_(status),
      owner_(nullptr), buyPrice_(buyPrice), mortgageValue_(mortgageValue),
      festivalMult_(1), festivalDur_(0) {
    if (code_.empty()) {
        throw InvalidGameStateException("Property code cannot be empty");
    }
    if (name_.empty()) {
        throw InvalidGameStateException("Property name cannot be empty");
    }
    if (buyPrice_ < 0 || mortgageValue_ < 0) {
        throw InvalidGameStateException(
            "Property price values must be non-negative");
    }
}

// Returns immutable property code.
const std::string& Property::getCode() const {
    return code_;
}

// Returns immutable property display name.
const std::string& Property::getName() const {
    return name_;
}

// Returns purchase price used when buying from bank.
int Property::getBuyPrice() const {
    return buyPrice_;
}

// Returns cash received when mortgaging/selling to bank.
int Property::getMortgageValue() const {
    return mortgageValue_;
}

// Returns current ownership lifecycle state.
PropertyStatus Property::getStatus() const {
    return status_;
}

// Overrides property status directly for orchestration logic.
void Property::setStatus(PropertyStatus status) {
    status_ = status;
}

// Returns current owner pointer, or null when owned by bank.
Player* Property::getOwner() const {
    return owner_;
}

// Sets owner and synchronizes status between BANK and OWNED.
void Property::setOwner(Player* owner, TurnContext& ctx) {
    owner_ = owner;

    // Update status dasar kepemilikan
    if (owner_ == nullptr) {
        status_ = PropertyStatus::BANK;
    } else if (status_ == PropertyStatus::BANK) {
        status_ = PropertyStatus::OWNED;
    }

    // Cek apakah properti ini adalah StreetProperty
    StreetProperty* currentStreet = dynamic_cast<StreetProperty*>(this);
    if (currentStreet == nullptr) {
        return;
    }

    recomputeStreetMonopolyForColor(ctx, currentStreet->getColor());
}

// Moves property from OWNED to MORTGAGED.
void Property::mortgage() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException("Only owned property can be mortgaged");
    }
    status_ = PropertyStatus::MORTGAGED;
}

// Restores property from MORTGAGED to OWNED.
void Property::redeem() {
    if (status_ != PropertyStatus::MORTGAGED) {
        throw InvalidGameStateException(
            "Only mortgaged property can be redeemed");
    }
    status_ = PropertyStatus::OWNED;
}

int Property::sellToBank() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException("Can only sell owned property to bank");
    }
    owner_ = nullptr;
    status_ = PropertyStatus::BANK;
    return buyPrice_;
}

// Compares properties by unique code identity.
bool Property::operator==(const Property& other) const {
    return code_ == other.code_;
}

// Applies festival effect with capped stack and duration reset.
void Property::applyFestival() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException(
            "Festival can only be applied to owned street property");
    }

    festivalMult_ = std::min(festivalMult_ * 2, 8);
    festivalDur_ = 3;
}

// Ticks festival duration each owner turn and clears effect when expired.
void Property::decreaseFestivalDuration() {
    if (festivalDur_ <= 0) {
        return;
    }

    --festivalDur_;
    if (festivalDur_ == 0) {
        festivalMult_ = 1;
    }
}

// Returns current festival rent multiplier.
int Property::getFestivalMultiplier() const {
    return festivalMult_;
}

// Returns remaining owner turns of festival effect.
int Property::getFestivalDuration() const {
    return festivalDur_;
}
