#pragma once

#include <functional>
#include <map>
#include <string>

#include "property/Property.hpp"

class RailroadProperty : public Property {
  public:
    RailroadProperty(std::string code,
                     std::string name,
                     int buyPrice, //nanti dihapus
                     int mortgageValue,
                     std::map<int, int> rentTable,
                      PropertyStatus status = PropertyStatus::BANK,
                      int festivalMult = 1,
                      int festivalDur = 0);

    int getRent(const TurnContext& ctx) const override;

    int getRentFromOwnedCount(int ownedRailroads) const;

    // Allows integration without hard-coupling this module to Player internals.
    void setOwnedRailroadCounter(std::function<int(const Player*)> counter);

    void printStatus(TurnContext& ctx) override;

  private:
    std::map<int, int> rentTable_;
    std::function<int(const Player*)> ownedRailroadCounter_;
};
