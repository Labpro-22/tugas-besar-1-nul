#include "property/Property.hpp"

#include <utility>

#include "exception/InvalidGameStateException.hpp"

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

const std::string& Property::getCode() const {
    return code_;
}

const std::string& Property::getName() const {
    return name_;
}

int Property::getBuyPrice() const {
    return buyPrice_;
}

int Property::getMortgageValue() const {
    return mortgageValue_;
}

PropertyStatus Property::getStatus() const {
    return status_;
}

void Property::setStatus(PropertyStatus status) {
    status_ = status;
}

Player* Property::getOwner() const {
    return owner_;
}

void Property::setOwner(Player* owner) {
    owner_ = owner;

    if (owner_ == nullptr) {
        status_ = PropertyStatus::BANK;
    } else if (status_ == PropertyStatus::BANK) {
        status_ = PropertyStatus::OWNED;
    }
}

void Property::mortgage() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException("Only owned property can be mortgaged");
    }
    status_ = PropertyStatus::MORTGAGED;
}

void Property::redeem() {
    if (status_ != PropertyStatus::MORTGAGED) {
        throw InvalidGameStateException(
            "Only mortgaged property can be redeemed");
    }
    status_ = PropertyStatus::OWNED;
}

int Property::sellToBank() {
    owner_ = nullptr;
    status_ = PropertyStatus::BANK;
    return mortgageValue_;
}

bool Property::operator==(const Property& other) const {
    return code_ == other.code_;
}
