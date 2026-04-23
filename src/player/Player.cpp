#include "player/Player.hpp"
#include <iostream>

#include "card/SkillCard.hpp"
#include "core/BankruptcyManager.hpp"
#include "exception/BankruptcyException.hpp"
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

int Player::getPosition() const { return this->position; }

bool Player::isInJail() const { return this->getStatus()==PlayerStatus::JAILED; }

int Player::getDiscountRate() const { return this->discountRate; }


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
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot buy null property");
    }
    if (this->getBalance() < p->getBuyPrice()) {
        throw InsufficientFundsException("Not enough money to buy " + 
                                         p->getName());
    }
    this->deductCash(p->getBuyPrice());
    this->addProperty(p);
}

void Player::sell(Property& p) {
    // double check exception
    if (this->getPropertiesAmount() < 1) { 
        // throw EmptyProperties
    }
    this->removeProperty(p);
}

void Player::mortgage(Property* p) {
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot mortgage null property");
    }
    if (p->getStatus() != PropertyStatus::OWNED) {
        throw InvalidGameStateException("Can only mortgage owned properties");
    }
    p->mortgage();
    this->addCash(p->getMortgageValue());
}

// ni nnti make sure parameter type
void Player::addProperty(Property* p) {
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot add null property");
    }
    p->setOwner(this);
    this->properties.push_back(p);

}

//make sure lgi type p
void Player::removeProperty(Property& p) {
    auto it = std::find_if(
        this->properties.begin(), this->properties.end(),
        [&p](const Property* prop) { return prop != nullptr && *prop == p; });
    if (it != this->properties.end()) {
        this->properties.erase(it);
    }
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

void Player::discardSCard(int idx) {
    if (idx < 0) {
        throw InvalidGameStateException("Card index cannot be negative, got: " + std::to_string(idx));
    }
    if (idx >= static_cast<int>(this->hand.size())) {
        throw InvalidGameStateException("Card index out of range: " + std::to_string(idx) + ", hand size: " + std::to_string(this->hand.size()));
    }
    this->hand.erase(this->hand.begin() + idx);
}

void Player::useSCard(int idx, TurnContext& ctx) {
    if (idx < 0) {
        throw InvalidGameStateException("Card index cannot be negative, got: " + std::to_string(idx));
    }
    if (idx >= static_cast<int>(this->hand.size())) {
        throw InvalidGameStateException("Card index out of range: " + std::to_string(idx) + ", hand size: " + std::to_string(this->hand.size()));
    }
    if (this->usedSkillThisTurn) {
        throw InvalidGameStateException("Skill card already used this turn");
    }
    // TODO: Implement actual card usage logic
    this->usedSkillThisTurn = true;
}

void Player::setDiscountRate(int discount) {
    if (discount < 0) {
        throw InvalidGameStateException("Discount rate cannot be negative, got: " + std::to_string(discount));
    }
    if (discount > 100) {
        throw InvalidGameStateException("Discount rate cannot exceed 100%, got: " + std::to_string(discount));
    }
    this->discountRate = discount;
}

void Player::activateShield() {
    if (this->isShieldActive) {
        throw InvalidGameStateException("Shield is already active for player " + this->username);
    }
    this->isShieldActive = true;
}

void Player::enterJail() { this->status = PlayerStatus::JAILED; this->jailTurns = 3; }

void Player::exitJail() { this->status = PlayerStatus::ACTIVE; }

void Player::decideAction(TurnContext& ctx) { 

}

bool Player::isBot() const {
    return false;
}

void Player::showHands() {
    std::cout << "=== Kartu di Tangan ===" << std::endl;
    if (hand.empty()) {
        std::cout << "Tidak ada kartu." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << (i + 1) << ". " << hand[i]->getDescription() << std::endl;
    }
}

void Player::profile() {
    std::cout << "=== Profil Pemain ===" << std::endl;
    std::cout << "Nama: " << username << std::endl;
    std::cout << "Uang: M" << balance << std::endl;
    std::cout << "Posisi: " << position << std::endl;
    std::cout << "Status: " << (status == PlayerStatus::ACTIVE ? "Aktif" : 
                               status == PlayerStatus::JAILED ? "Dipenjara" : "Bangkrut") << std::endl;
    std::cout << "Jumlah Properti: " << properties.size() << std::endl;
    std::cout << "Jumlah Kartu: " << hand.size() << std::endl;
}

void Player::showProperties() {
    std::cout << "=== Properti Milik " << username << " ===" << std::endl;
    if (properties.empty()) {
        std::cout << "Tidak memiliki properti." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < properties.size(); ++i) {
        Property* prop = properties[i];
        std::cout << (i + 1) << ". " << prop->getName() 
                  << " (" << prop->getCode() << ") - ";
        
        switch (prop->getStatus()) {
            case PropertyStatus::OWNED:
                std::cout << "OWNED";
                break;
            case PropertyStatus::MORTGAGED:
                std::cout << "MORTGAGED [M]";
                break;
            default:
                std::cout << "UNKNOWN";
        }
        std::cout << std::endl;
    }
}

void Player::addCash(int amount) {
    if (amount <= 0) {
        throw InvalidGameStateException("Cannot add non-positive cash amount: " + std::to_string(amount));
    }
    this->balance += amount;
}

void Player::deductCash(int amount) {
    if (amount <= 0) {
        throw InvalidGameStateException("Cannot deduct non-positive cash amount: " + std::to_string(amount));
    }
    if (this->balance < amount) {
        throw InsufficientFundsException("Insufficient funds for payment of " +
                                          std::to_string(amount));
    }
    this->balance -= amount;
}

void Player::setBankruptStatus() {
    if (this->status == PlayerStatus::BANKRUPT) {
        throw InvalidGameStateException("Player " + this->username + " is already declared bankrupt");
    }
    this->status = PlayerStatus::BANKRUPT;
}

bool Player::checkBankruptcy(int requiredAmount) {
    if (requiredAmount <= 0) {
        throw InvalidGameStateException("Required amount must be positive, got: " + std::to_string(requiredAmount));
    }

    if (this->status == PlayerStatus::BANKRUPT) {
        throw InvalidGameStateException("Player " + this->username + " is already bankrupt");
    }

    if (this->balance >= requiredAmount) {
        return false;  
    }

    if (!BankruptcyManager::canCoverDebt(this, requiredAmount)) {
        BankruptcyManager::declareBankrupt(this);
        return true;
    }

    return false;
}

