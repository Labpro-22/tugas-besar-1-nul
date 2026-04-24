#pragma once

#include <string>
#include <vector>
// using namespace std;

class Property;
class SkillCard;
class TurnContext;

enum class PlayerStatus {
    ACTIVE,
    JAILED,
    BANKRUPT
};

class Player {
protected:
    /* general */
    std::string username;
    PlayerStatus status;
    int balance;
    int position;
    int jailTurns;

    /* skills */
    bool usedSkillThisTurn;
    int discountRate; // DiscountCard
    bool hasShield; // ShieldCard

    /* containers */
    std::vector<Property*> properties;
    std::vector<SkillCard*> hand;

public:
    // apakah mau diimplement ato jadi buysell aja as a whole
    void addProperty(Property* p);
    void removeProperty(Property& p); //make sure lgi type p

// public:
    // ctor dtor
    Player(std::string username, int balance);
    ~Player();

    // default getter
    std::string getUsername() const;
    PlayerStatus getStatus() const;
    int getBalance() const;
    bool isInJail() const;
    bool isShieldActive() const;
    int getDiscountRate() const;
    int getPosition() const;
    int getPropertiesAmount() const;
    int getHandsAmount() const;
    int getWealth() const;
    const std::vector<Property*>& getProperties() const;

    // op
    bool operator>=(const Player& other);
    bool operator<(const Player& other);

    // moving
    int move(int steps, TurnContext& ctx);
    void moveForwardTo(int index, TurnContext& ctx);
    void moveBackwardTo(int index, TurnContext& ctx);
    // void onLanded(TurnContext& ctx);

    // properties
    void buy(Property* p);
    void buy(Property* p, int buyAmount);
    bool upgrade(TurnContext& ctx);
    void sell(Property& p);
    void mortgage(Property* p);

    // cash management and bankruptcy
    void addCash(int amount);
    void deductCash(int amount);
    void setBankruptStatus();
    bool checkBankruptcy(int requiredAmount);

    // hands or cards
    void drawSCard(SkillCard* deck);
    void discardSCard(int idx);
    void useSCard(int idx, TurnContext& ctx);
    void setDiscountRate(int discount);
    void activateShield();
    void deactivateShield();

    // jail
    void enterJail();
    void exitJail();

    // action
    virtual void decideAction(TurnContext& ctx);

    // Check if player is a bot (for polymorphism)
    virtual bool isBot() const;

    // printing
    void showProperties();
    void showHands();
    void profile();
};