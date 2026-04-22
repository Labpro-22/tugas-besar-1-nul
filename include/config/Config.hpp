#pragma once

#include <vector>

#include "config/MiscConfig.hpp"
#include "config/PropertyData.hpp"
#include "config/RailroadConfig.hpp"
#include "config/SpecialConfig.hpp"
#include "config/TaxConfig.hpp"
#include "config/UtilityConfig.hpp"

class Config {
  public:
    std::vector<PropertyData> properties;
    RailroadConfig railroad;
    UtilityConfig utility;
    TaxConfig tax;
    SpecialConfig special;
    MiscConfig misc;
};
