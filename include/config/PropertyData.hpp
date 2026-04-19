#pragma once

#include <string>
#include <vector>

class PropertyData {
  public:
    int id = 0;
    std::string code;
    std::string name;
    std::string type;
    std::string color;
    int buyPrice = 0;
    int mortgageValue = 0;
    int houseUpgrade = 0;
    int hotelUpgrade = 0;
    std::vector<int> rentTable;
};
