#include "player/Player.h"
#include "exception/InsufficientFundsException.h" //nanti ganti ke yg general

// enum class PlayerStatus {
//     ACTIVE,
//     JAILED,
//     BANKRUPT
// };

    // string username;
    // PlayerStatus status;
    
    // int balance;
    // int position;
    // int jailTurns;
    // bool usedSkillThisTurn;

    // vector<Property*> properties;
    // vector<SkillCard*> hand;

/* ctor dtor */
Player::Player(std::string username, int balance) : username(username), status(PlayerStatus::ACTIVE), balance(balance), position(0), jailTurns(0), usedSkillThisTurn(false), properties({}), hand({}) {}
Player::~Player() {}

/* getter */
std::string Player::getUsername() const { return this->username; }

PlayerStatus Player::getStatus() const { return this->status; }

int Player::getBalance() const { return this->balance; }

bool Player::isInJail() const { return this->getStatus()==PlayerStatus::JAILED; }

int Player::getPropertiesAmount() const { return this->properties.size(); }

int Player::getHandsAmount() const { return this->hand.size(); }

int Player::getWealth() const {
    return 0; //pastiin
}

/* op */
bool Player::operator>=(const Player& other) {
    if (this->balance == other.balance) {
        if (this->getPropertiesAmount() == other.getPropertiesAmount()) {
            // kalo sama itung jg
            return this->getHandsAmount() >= other.getHandsAmount(); 
        }
        return this->getPropertiesAmount() > other.getPropertiesAmount();
    }
    return this->getBalance() > other.getBalance();
}

bool Player::operator<(const Player& other) {
    if (this->balance == other.balance) {
        if (this->getPropertiesAmount() == other.getPropertiesAmount()) {
            return this->getHandsAmount() < other.getHandsAmount();
        }
        return this->getPropertiesAmount() < other.getPropertiesAmount();
    }
    return this->getBalance() < other.getBalance();
}

/* moving */
void Player::move(int steps, int size) {
    this->position %= (this->position + steps)%size;
}

void Player::moveForwardTo(int index, int size) {
    this->position = index;
    // ni nanti implement ngapain tilenya, kalo start jg
}

void Player::moveBackwardTo(int index, int size) {
    this->position = index;
    // ni nanti implement ngapain tilenya
}

/* properties */
void Player::buy(Property* p) {
    // double check property
    if (this->getBalance() < p->cost) { throw InsufficientFundsException("Not enough money!"); }
    this->addProperty(p);
}

void Player::sell(Property& p) {
    // double check exception
    if (this->getPropertiesAmount() < 1) { 
        // throw EmptyProperties
    }
    this->removeProperty(p);
}

// ni nnti make sure parameter type
void Player::addProperty(Property* p) {
    this->properties.push_back(p);
}

//make sure lgi type p
void Player::removeProperty(Property& p) {
    // std::_Erase_nodes_if(this->properties, [](const Properties* props) {
    //     return props->name == p->name;
    // })
    // cross check
}

void Player::drawSCard(SkillCard* card) {}

void Player::discardSCard(int idx) {}

void Player::useSCard(int idx, TurnContext& ctx) {}

void Player::enterJail() { this->status = PlayerStatus::JAILED; this->jailTurns = 3; }
void Player::exitJail() { this->status = PlayerStatus::ACTIVE; }

void Player::decideAction(TurnContext& ctx) { 

}

