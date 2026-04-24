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
                   std::vector<int> rentTable,
                   PropertyStatus ps = PropertyStatus::BANK,
                   int festivalMul = 1,
                   int festivalDur = 0);

    int getRent(const TurnContext& ctx) const override;
    int sellToBank();

    const std::string& getColorGroup() const;

    int getBuildingCount() const;
    bool hasHotel() const;
    int getHousePrice() const;
    int getHotelPrice() const;

    const std::string getColor() const;

    const std::vector<int>& getRentTable() const;

    bool isMonopolized() const;
    void setMonopolized(bool monopolized);

    bool canBuild() const;
    void buildHouse();
    void upgradeToHotel();
    void demolish();

    // Restore building state from save file (deserialization helper)
    void restoreBuildings(int count, bool hotel);

    void printStatus(TurnContext& ctx) override;
    void printRentTable();

  private:
    std::string colorGroup_;
    int buildingCount_;
    bool isHotel_;
    int housePrice_;
    int hotelPrice_;
    std::vector<int> rentTable_;
    bool monopolized_;
};
