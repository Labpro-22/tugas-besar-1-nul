#pragma once

#include <string>
#include <vector>

#include "../../include/property/Property.hpp"

class StreetProperty : public Property {
  public:
    StreetProperty(std::string code,
                   std::string name,
                   int buyPrice,
                   int mortgageValue,
                   std::string colorGroup,
                   int housePrice,
                   int hotelPrice,
                   std::vector<int> rentTable);

    int getRent(const TurnContext& ctx) const override;

    const std::string& getColorGroup() const;

    int getBuildingCount() const;
    bool hasHotel() const;
    int getHousePrice() const;
    int getHotelPrice() const;

    int getFestivalMultiplier() const;
    int getFestivalDuration() const;

    bool isMonopolized() const;
    void setMonopolized(bool monopolized);

    bool canBuild() const;
    void buildHouse();
    void upgradeToHotel();
    void demolish();

    void applyFestival();
    void decreaseFestivalDuration();

    void printStatus() override;

  private:
    std::string colorGroup_;
    int buildingCount_;
    bool isHotel_;
    int housePrice_;
    int hotelPrice_;
    std::vector<int> rentTable_;
    int festivalMult_;
    int festivalDur_;
    bool monopolized_;
};
