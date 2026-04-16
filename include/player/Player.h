#pragma once

#include <string>
#include <vector>
// using namespace std;

enum class PlayerStatus {
    ACTIVE,
    JAILED,
    BANKRUPT
};

class Player {
protected:
    std::string username;
    PlayerStatus status;
    
    int balance;
    int position;
    int jailTurns;
    bool usedSkillThisTurn;

    std::vector<Property*> properties;
    std::vector<SkillCard*> hand;

private:
    // apakah mau diimplement ato jadi buysell aja as a whole
    void addProperty(Property* p);
    void removeProperty(Property& p); //make sure lgi type p

public:
    // ctor dtor
    Player(std::string username, int balance);
    ~Player();

    // default getter
    std::string getUsername() const;
    PlayerStatus getStatus() const;
    int getBalance() const;
    bool isInJail() const;
    int getPropertiesAmount() const;
    int getHandsAmount() const;
    int getWealth() const;

    // op
    bool operator>=(const Player& other);
    bool operator<(const Player& other);

    // moving
    void move(int steps, int size);
    void moveForwardTo(int index, int size);
    void moveBackwardTo(int index, int size);

    // properties
    void buy(Property* p);
    void sell(Property& p);
    void mortgage(Property* p);

    // hands or cards
    void drawSCard(SkillCard* deck);
    void discardSCard(int idx);
    void useSCard(int idx, TurnContext& ctx);

    // jail
    void enterJail();
    void exitJail();

    // action
    void decideAction(TurnContext& ctx);

    // printing
    void showProperties();
    void showHands();
    void profile();
};