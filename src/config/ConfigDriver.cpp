#include "config/ConfigDriver.hpp"

#include <iostream>

#include "config/Config.hpp"
#include "core/ConfigLoader.hpp"

namespace {

void printProperty(const PropertyData& property) {
    std::cout << "- ID=" << property.id << ", CODE=" << property.code
              << ", NAME=" << property.name << ", TYPE=" << property.type
              << ", COLOR=" << property.color << ", BUY=" << property.buyPrice
              << ", MORTGAGE=" << property.mortgageValue
              << ", UPG_HOUSE=" << property.houseUpgrade
              << ", UPG_HOTEL=" << property.hotelUpgrade << ", RENT=[";

    for (std::size_t i = 0; i < property.rentTable.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << property.rentTable[i];
    }

    std::cout << "]\n";
}

void printIntMap(const std::map<int, int>& m, const std::string& title) {
    std::cout << title << " (" << m.size() << " entries)\n";
    for (const auto& [k, v] : m) {
        std::cout << "  " << k << " -> " << v << "\n";
    }
}

} // namespace

void runConfigLoaderDriver(const std::string& basePath) {
    ConfigLoader loader;
    Config cfg = loader.loadAll(basePath);

    std::cout << "CONFIG DRIVER\n";
    std::cout << "Base path: " << basePath << "\n\n";

    std::cout << "[Properties] count = " << cfg.properties.size() << "\n";
    for (const PropertyData& p : cfg.properties) {
        printProperty(p);
    }
    std::cout << "\n";

    printIntMap(cfg.railroad.rentTable, "[Railroad rent table]");
    std::cout << "\n";

    printIntMap(cfg.utility.multiplierTable, "[Utility multiplier table]");
    std::cout << "\n";

    std::cout << "[Tax]\n";
    std::cout << "  pphFlat=" << cfg.tax.pphFlat
              << ", pphPercent=" << cfg.tax.pphPercent
              << ", pbmFlat=" << cfg.tax.pbmFlat << "\n\n";
}
