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
    /* misc */
    const int* boardSizeSource; // non-owning pointer (ngikut RAII krn ga manage lgsg)

    /* general */
    std::string username;
    PlayerStatus status;
    int balance;
    int position;
    int jailTurns;

    /* skills */
    bool usedSkillThisTurn;
    int discountRate; // DiscountCard
    bool isShieldActive; // ShieldCard

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
    void setBoardSizeSource(int* sizeSource);
    void clearBoardSizeSource();
    void move(int steps);
    void moveForwardTo(int index);
    void moveBackwardTo(int index);

    // properties
    void buy(Property* p);
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