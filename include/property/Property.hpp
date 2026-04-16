#pragma once

#include <string>

#include "property/PropertyStatus.hpp"

class Player;
class TurnContext;

class Property {
  public:
    Property(std::string code,
             std::string name,
             int buyPrice,
             int mortgageValue,
             PropertyStatus status = PropertyStatus::BANK);

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

  protected:
    std::string code_;
    std::string name_;
    PropertyStatus status_;
    Player* owner_;
    int buyPrice_;
    int mortgageValue_;
};
