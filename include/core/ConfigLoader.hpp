#pragma once

#include <string>
#include <vector>

#include "config/ActionTileData.hpp"
#include "config/Config.hpp"
#include "config/MiscConfig.hpp"
#include "config/PropertyData.hpp"
#include "config/RailroadConfig.hpp"
#include "config/SpecialConfig.hpp"
#include "config/TaxConfig.hpp"
#include "config/UtilityConfig.hpp"

class ConfigLoader {
  public:
    Config loadAll(const std::string& path) const;
    std::vector<PropertyData> loadProperty(const std::string& file) const;
    std::vector<ActionTileData> loadActionTiles(const std::string& file) const;
    RailroadConfig loadRailroad(const std::string& file) const;
    UtilityConfig loadUtility(const std::string& file) const;
    TaxConfig loadTax(const std::string& file) const;
    SpecialConfig loadSpecial(const std::string& file) const;
    MiscConfig loadMisc(const std::string& file) const;
};
