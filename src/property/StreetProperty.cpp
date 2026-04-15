#include "property/StreetProperty.hpp"

#include <algorithm>
#include <utility>

#include "exception/InvalidGameStateException.hpp"

StreetProperty::StreetProperty(std::string code,
                               std::string name,
                               int buyPrice,
                               int mortgageValue,
                               std::string colorGroup,
                               int housePrice,
                               int hotelPrice,
                               std::vector<int> rentTable)
    : Property(std::move(code), std::move(name), buyPrice, mortgageValue),
      colorGroup_(std::move(colorGroup)), buildingCount_(0), isHotel_(false),
      housePrice_(housePrice), hotelPrice_(hotelPrice),
      rentTable_(std::move(rentTable)), festivalMult_(1), festivalDur_(0),
      monopolized_(false) {
    if (colorGroup_.empty()) {
        throw InvalidGameStateException(
            "StreetProperty color group cannot be empty");
    }
    if (housePrice_ < 0 || hotelPrice_ < 0) {
        throw InvalidGameStateException(
            "Building upgrade prices must be non-negative");
    }
    if (rentTable_.size() < 6) {
        throw InvalidGameStateException(
            "StreetProperty rent table must contain L0-L5 values");
    }
}

int StreetProperty::getRent(const TurnContext& ctx) const {
    (void) ctx;

    if (status_ != PropertyStatus::OWNED) {
        return 0;
    }

    const int level = std::clamp(buildingCount_, 0, 5);
    const int baseRent = rentTable_[static_cast<std::size_t>(level)];
    return baseRent * festivalMult_;
}

const std::string& StreetProperty::getColorGroup() const {
    return colorGroup_;
}

int StreetProperty::getBuildingCount() const {
    return buildingCount_;
}

bool StreetProperty::hasHotel() const {
    return isHotel_;
}

int StreetProperty::getHousePrice() const {
    return housePrice_;
}

int StreetProperty::getHotelPrice() const {
    return hotelPrice_;
}

int StreetProperty::getFestivalMultiplier() const {
    return festivalMult_;
}

int StreetProperty::getFestivalDuration() const {
    return festivalDur_;
}

bool StreetProperty::isMonopolized() const {
    return monopolized_;
}

void StreetProperty::setMonopolized(bool monopolized) {
    monopolized_ = monopolized;
}

bool StreetProperty::canBuild() const {
    return status_ == PropertyStatus::OWNED && monopolized_ && !isHotel_;
}

void StreetProperty::buildHouse() {
    if (!canBuild()) {
        throw InvalidGameStateException(
            "Cannot build house on this street property");
    }
    if (buildingCount_ >= 4) {
        throw InvalidGameStateException(
            "House count already at max for non-hotel state");
    }
    ++buildingCount_;
}

void StreetProperty::upgradeToHotel() {
    if (!canBuild()) {
        throw InvalidGameStateException(
            "Cannot upgrade to hotel on this street property");
    }
    if (buildingCount_ != 4) {
        throw InvalidGameStateException(
            "Need exactly 4 houses before upgrading to hotel");
    }
    buildingCount_ = 5;
    isHotel_ = true;
}

void StreetProperty::demolish() {
    if (buildingCount_ <= 0) {
        throw InvalidGameStateException("No building available to demolish");
    }

    if (isHotel_) {
        isHotel_ = false;
        buildingCount_ = 4;
        return;
    }

    --buildingCount_;
}

void StreetProperty::applyFestival() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException(
            "Festival can only be applied to owned street property");
    }

    festivalMult_ = std::min(festivalMult_ * 2, 8);
    festivalDur_ = 3;
}

void StreetProperty::decreaseFestivalDuration() {
    if (festivalDur_ <= 0) {
        return;
    }

    --festivalDur_;
    if (festivalDur_ == 0) {
        festivalMult_ = 1;
    }
}
