#pragma once

#include <vector>

#include "config/ActionTileData.hpp"
#include "config/MiscConfig.hpp"
#include "config/PropertyData.hpp"
#include "config/RailroadConfig.hpp"
#include "config/SpecialConfig.hpp"
#include "config/TaxConfig.hpp"
#include "config/UtilityConfig.hpp"

class Config {
  public:
    std::vector<PropertyData> properties;
    std::vector<ActionTileData> actionTiles;
    RailroadConfig railroad;
    UtilityConfig utility;
    TaxConfig tax;
    SpecialConfig special;
    MiscConfig misc;
};
