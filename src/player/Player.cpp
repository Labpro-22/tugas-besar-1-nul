#include "player/Player.hpp"
#include <iostream>

#include "card/SkillCard.hpp"
#include "exception/CardSlotFullException.hpp"
#include "exception/InsufficientFundsException.hpp" //nanti ganti ke yg general
#include "exception/InvalidGameStateException.hpp"
#include "property/Property.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"

/* === CTOR DTOR === */
Player::Player(std::string username, int balance)
        : username(username),
            status(PlayerStatus::ACTIVE),
            balance(balance),
            position(0),
            jailTurns(0),
            usedSkillThisTurn(false),
            discountRate(0),
            properties({}),
            hand({}) {}

Player::~Player() {}




/* === GETTERS === */
std::string Player::getUsername() const { return this->username; }

PlayerStatus Player::getStatus() const { return this->status; }

int Player::getBalance() const { return this->balance; }

bool Player::isInJail() const { return this->getStatus()==PlayerStatus::JAILED; }

int Player::getDiscountRate() const { return this->discountRate; }

int Player::getPosition() const { return this->position; }

int Player::getPropertiesAmount() const { return this->properties.size(); }

int Player::getHandsAmount() const { return this->hand.size(); }

const std::vector<Property*>& Player::getProperties() const { return this->properties; }

int Player::getWealth() const {
    return 0; //pastiin
}




/* === OPERATORS === */
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




/* === MOVING === */
int Player::move(int steps, TurnContext& ctx) {
    // if (ctx.board.getSize() == nullptr) { throw InvalidGameStateException("Board size source is not set for player movement.");}
    if (ctx.board.getSize() <= 0) { throw InvalidGameStateException("Board size must be positive."); }
    const int trueSteps = ((steps % ctx.board.getSize()) + ctx.board.getSize()) % ctx.board.getSize();
    this->position = (this->position + trueSteps) % ctx.board.getSize();
    return this->position;
}

void Player::moveForwardTo(int index, TurnContext& ctx) {
    this->position = index;
    // ni nanti implement ngapain tilenya, kalo start jg
}

void Player::moveBackwardTo(int index, TurnContext& ctx) {
    this->position = index;
    // ni nanti implement ngapain tilenya
}




/* === PROPERTIES === */
void Player::buy(Property* p) {
    // double check property
    if (this->getBalance() < p->getBuyPrice()) { throw InsufficientFundsException("Not enough money!"); }
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




/* === SKILL CARDS === */
void Player::drawSCard(SkillCard* card) {
    if (card == nullptr) {
        throw InvalidGameStateException("Cannot draw a null skill card.");
    }
    if (this->hand.size() >= 3) {
        throw CardSlotFullException("Skill card hand is full (max 3 cards).");
    }

    this->hand.push_back(card);
    std::cout << "[MENDAPATKAN KEMAMPUAN] " << card->getDescription() << " ditambahkan ke tangan.\n";
}

void Player::discardSCard(int idx) {}

void Player::useSCard(int idx, TurnContext& ctx) {}

void Player::setDiscountRate(int discount) { this->discountRate = discount; }

void Player::activateShield() { this->isShieldActive = true; }

void Player::enterJail() { this->status = PlayerStatus::JAILED; this->jailTurns = 3; }

void Player::exitJail() { this->status = PlayerStatus::ACTIVE; }

void Player::decideAction(TurnContext& ctx) { 

}

