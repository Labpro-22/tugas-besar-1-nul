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

} // namespace

void runConfigLoaderDriver(const std::string& basePath) {
    ConfigLoader loader;
    Config config = loader.loadAll(basePath);

    std::cout << "[CONFIG DRIVER] Loaded config from: " << basePath << "\n";
    std::cout << "[CONFIG DRIVER] Property count: " << config.properties.size()
              << "\n";

    const std::size_t previewCount =
        config.properties.size() < 8 ? config.properties.size() : 8;

    std::cout << "[CONFIG DRIVER] Preview first " << previewCount
              << " properties:\n";
    for (std::size_t i = 0; i < previewCount; ++i) {
        printProperty(config.properties[i]);
    }

    /* std::cout << "[CONFIG DRIVER] Tax config: pphFlat=" << config.tax.pphFlat
              << ", pphPercent=" << config.tax.pphPercent
              << ", pbmFlat=" << config.tax.pbmFlat << "\n";
*/
    /*
    std::cout << "[CONFIG DRIVER] Special config: goSalary="
              << config.special.goSalary
              << ", jailFine=" << config.special.jailFine << "\n";
*/

    /*
        std::cout << "[CONFIG DRIVER] Misc config: maxTurn=" <<
       config.misc.maxTurn
                  << ", startingBalance=" << config.misc.startingBalance <<
       "\n";
                  */
}
