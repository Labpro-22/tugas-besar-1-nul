#pragma once

#include <string>

class PropertyState {
  public:
    std::string code;
    std::string name;
    std::string type;
    std::string colorGroup;
    std::string owner;
    int ownerPlayerIdx = -1;
    std::string status;
    int festivalMult = 0;
    int festivalDur = 0;
    int buildingCount = 0;
    bool isHotel = false;
};
