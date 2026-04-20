#include "property/Property.hpp"

#include <utility>

#include "exception/InvalidGameStateException.hpp"

// Initializes common property state and validates basic invariants.
Property::Property(std::string code,
                   std::string name,
                   int buyPrice,
                   int mortgageValue,
                   PropertyStatus status)
    : code_(std::move(code)), name_(std::move(name)), status_(status),
      owner_(nullptr), buyPrice_(buyPrice), mortgageValue_(mortgageValue) {
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
void Property::setOwner(Player* owner) {
    owner_ = owner;

    if (owner_ == nullptr) {
        status_ = PropertyStatus::BANK;
    } else if (status_ == PropertyStatus::BANK) {
        status_ = PropertyStatus::OWNED;
    }
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
