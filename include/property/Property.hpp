#pragma once

#include <string>

// #include "property/PropertyStatus.hpp"

#include "PropertyStatus.hpp"

class Player;
class TurnContext;

class Property {
  public:
    Property(std::string code,
             std::string name,
             int buyPrice,
             int mortgageValue,
             PropertyStatus status = PropertyStatus::BANK,
             int festivalMult = 1,
             int festivalDur = 0);

    virtual ~Property() = default;

    const std::string& getCode() const;
    const std::string& getName() const;
    int getBuyPrice() const;
    int getMortgageValue() const;

    PropertyStatus getStatus() const;
    void setStatus(PropertyStatus status);

    Player* getOwner() const;
    void setOwner(Player* owner);

    void mortgage();
    void redeem();

    // Sells this property back to bank and returns the cash value received.
    int sellToBank();

    virtual int getRent(const TurnContext& ctx) const = 0;

    bool operator==(const Property& other) const;

    virtual void printStatus(TurnContext& ctx) = 0;

    int getFestivalMultiplier() const;
    int getFestivalDuration() const;

    void applyFestival();
    void decreaseFestivalDuration();

  protected:
    std::string code_;
    std::string name_;
    PropertyStatus status_;
    Player* owner_; // ni hrs pointer gbs ref krn harus bisa null
    int buyPrice_;
    int mortgageValue_;
    int festivalMult_;
    int festivalDur_;
};
