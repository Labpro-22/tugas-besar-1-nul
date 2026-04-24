#include "property/StreetProperty.hpp"
#include "property/Property.hpp"
#include "player/Player.hpp"

#include <algorithm>
#include <utility>
#include <iostream>

#include "exception/InvalidGameStateException.hpp"

// Creates a street property and validates rent/build configuration.
StreetProperty::StreetProperty(std::string code,
                               std::string name,
                               int buyPrice,
                               int mortgageValue,
                               std::string colorGroup,
                               int housePrice,
                               int hotelPrice,
                               std::vector<int> rentTable,
                               PropertyStatus ps,
                               int festivalMult,
                               int festivalDur)
    : Property(std::move(code), std::move(name), buyPrice, mortgageValue, ps, festivalMult, festivalDur),
      colorGroup_(std::move(colorGroup)), buildingCount_(0), isHotel_(false),
      housePrice_(housePrice), hotelPrice_(hotelPrice),
      rentTable_(std::move(rentTable)), 
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

// Computes rent using building level and active festival multiplier.
int StreetProperty::getRent(const TurnContext& ctx) const {
    (void) ctx;

    if (status_ != PropertyStatus::OWNED) {
        return 0;
    }

    const int level = std::clamp(buildingCount_, 0, 5);
    const int baseRent = rentTable_[static_cast<std::size_t>(level)];
    return baseRent * festivalMult_;
}


int StreetProperty::sellToBank() {
    if (status_ != PropertyStatus::OWNED) {
        throw InvalidGameStateException(
            "Can only sell owned property to bank");
    }

    int buildingCost = 0;

    if (buildingCount_ > 0) {
        if (isHotel_) {
            buildingCost = (4 * housePrice_) + hotelPrice_;
        } else {
            buildingCost = buildingCount_ * housePrice_;
        }
    }

    // Demolish all buildings
    buildingCount_ = 0;
    isHotel_ = false;
    monopolized_ = false;

    owner_ = nullptr;
    status_ = PropertyStatus::BANK;

    return buyPrice_ + (buildingCost / 2);
}

// Returns color group used for monopoly/build validation.
const std::string& StreetProperty::getColorGroup() const {
    return colorGroup_;
}

// Returns current number of built levels (0-5).
int StreetProperty::getBuildingCount() const {
    return buildingCount_;
}

// Returns true when level 5 is represented as hotel state.
bool StreetProperty::hasHotel() const {
    return isHotel_;
}

// Returns house build cost from config.
int StreetProperty::getHousePrice() const {
    return housePrice_;
}

// Returns hotel upgrade cost from config.
int StreetProperty::getHotelPrice() const {
    return hotelPrice_;
}
const std::string StreetProperty::getColor() const{
    return colorGroup_;
}

// Returns whether full color-set ownership is satisfied.
bool StreetProperty::isMonopolized() const {
    return monopolized_;
}

// Updates monopoly flag (typically managed by board/engine checks).
void StreetProperty::setMonopolized(bool monopolized) {
    monopolized_ = monopolized;
}

// Checks if this street is eligible for adding buildings.
bool StreetProperty::canBuild() const {
    return status_ == PropertyStatus::OWNED && monopolized_ && !isHotel_;
}

// Adds one house level up to level 4.
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

// Converts four houses into a hotel (level 5).
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

// Removes one building level, or downgrades hotel back to 4 houses.
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

// Restore building state from save file (deserialization helper)
void StreetProperty::restoreBuildings(int count, bool hotel) {
    buildingCount_ = count;
    isHotel_ = hotel;
}
void StreetProperty::printStatus(TurnContext& ctx){
    //nanti perbaiki di trigger agar tunjukin kalau di daerah bukan sendiri
    std::cout << "+=============================================+\n";
    std::cout << "| [" << getColorGroup() << "] " << getName() << " (" << getCode() << ")\n";
    std::string stat = "BANK";
    if (getStatus() == PropertyStatus::OWNED){
        stat = "OWNED BY [" + getOwner()->getUsername() + "]";
    } else if (getStatus() == PropertyStatus::MORTGAGED){
        stat = "MORTGAGED BY [" + getOwner()->getUsername() + "]";
    }
    std::cout << "| Status: " << stat << "\n";
    std::cout << "| Harga Beli    : M" << getBuyPrice() << "\n";
    printRentTable();
    std::cout << "+=============================================+\n";
    if (getStatus() == PropertyStatus::OWNED){
        std::cout << "Harga saat ini : M" << getRent(ctx) << "\n";
    }
}

const std::vector<int>& StreetProperty::getRentTable() const{
    return rentTable_;
}

void StreetProperty::printRentTable(){
    std::vector<int> rt = getRentTable();
    for (int i = 0; i<rt.size(); i++){
        if (i < rt.size()-1){
            std::cout << "| Sewa dengan banyak rumah " << i << ": M" << rt[i] << "\n";
        } else{
            std::cout << "| Sewa dengan hotel: M" << rt[i] << "\n";
        }
    }
}