#include "player/Player.hpp"
#include <iostream>

#include "board/Board.hpp"
#include "card/SkillCard.hpp"
#include "core/AuctionManager.hpp"
#include "core/BankruptcyManager.hpp"
#include "core/TurnContext.hpp"
#include "exception/BankruptcyException.hpp"
#include "exception/CardSlotFullException.hpp"
#include "exception/InsufficientFundsException.hpp" //nanti ganti ke yg general
#include "exception/InvalidGameStateException.hpp"
#include "property/Property.hpp"
#include "property/RailroadProperty.hpp"
#include "property/StreetProperty.hpp"
#include "property/UtilityProperty.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"
#include "core/GameEngine.hpp"

/* === CTOR DTOR === */
Player::Player(std::string username, int balance)
        : username(username),
            status(PlayerStatus::ACTIVE),
            balance(balance),
            position(0),
            jailTurns(0),
            usedSkillThisTurn(false),
            discountRate(0),
            hasShield(false),
            properties({}),
            hand({}) {}


Player::~Player() {}

/* === GETTERS === */
std::string Player::getUsername() const { return this->username; }

PlayerStatus Player::getStatus() const { return this->status; }

int Player::getBalance() const { return this->balance; }

int Player::getPosition() const { return this->position; }

bool Player::isInJail() const { return this->getStatus()==PlayerStatus::JAILED; }

bool Player::isShieldActive() const { return this->hasShield; }

int Player::getDiscountRate() const { return this->discountRate; }

int Player::getJailTurns() const {return this->jailTurns; }

int Player::getPropertiesAmount() const { return this->properties.size(); }

int Player::getHandsAmount() const { return this->hand.size(); }

const std::vector<Property*>& Player::getProperties() const { return this->properties; }

const std::vector<SkillCard*>& Player::getHand() const { return this->hand; }

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
    const int boardSize = ctx.board.getSize();
    const int oldPosition = this->position;
    const int trueSteps = ((steps % boardSize) + boardSize) % boardSize;
    int nPassStart = 0;

    if (steps > 0) {
        nPassStart = (oldPosition + steps) / boardSize;
    }

    this->position = (oldPosition + trueSteps) % boardSize;

    for (int i = 0; i < nPassStart; ++i) {
        std::cout << "You crossed START. You accepted M" << ctx.gameEngine.getGoSalary() << "\n";
        this->addCash(ctx.gameEngine.getGoSalary());
        ctx.gameEngine.giveRandomSkillCardTo(*this);
    }

    return this->position;
}

void Player::moveForwardTo(int index, TurnContext& ctx) {
    this->position = index;
    Tile* baseTile = ctx.board.getTile(index);
    if (baseTile != nullptr) {
        baseTile->onLanded(ctx);
    }
}

void Player::moveBackwardTo(int index, TurnContext& ctx) {
    this->position = index;
    Tile* baseTile = ctx.board.getTile(index);
    if (baseTile != nullptr) {
        baseTile->onLanded(ctx);
    }
}

void Player::setPosition(int pos) {
    this->position = pos;
}




/* === PROPERTIES === */
void Player::buy(Property* p, TurnContext& ctx) {
    // double check property
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot buy null property");
    }
    if (this->getBalance() < p->getBuyPrice()) {
        throw InsufficientFundsException("Not enough money to buy " +
                                         p->getName());
    }
    this->deductCash(p->getBuyPrice());
    this->addProperty(p, ctx);
}

void Player::buy(Property* p, int buyAmount, TurnContext& ctx) {
    // double check property
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot buy null property");
    }
    // if (this->getBalance() < p->getBuyPrice()) {
    //     throw InsufficientFundsException("AUCTION: Not enough money to buy " + 
    //                                      p->getName()); //should never appear due to auction characteristics
    // }
    try{
        this->deductCash(buyAmount);
    } catch (NimonspoliException* e){
        cout << e->what() << "\n";
        AuctionManager am;
        TurnManager tm = ctx.getTurnMgr();
        AuctionWinner aw = am.runAuction(*p, tm.getTurnOrder(), tm.getActivePlayerIndex());
        aw.winner.buy(&aw.prop_won, aw.buyAmount, ctx);
        cout << "\n";
        cout << "[" << aw.winner.getUsername() << "] just won the auction for " << aw.prop_won.getName() << "\n";
        cout << "[" << aw.winner.getUsername() << "]'s remaining money: " << aw.winner.getBalance() << "\n\n";
    }
    this->addProperty(p, ctx);
}

bool Player::upgrade(TurnContext& ctx) {
    std::vector<StreetProperty*> eligible_props;

    // Kumpulkan properti yang BENAR-BENAR bisa dibangun saat ini
    for (Property* prop : properties) {
        StreetProperty* sp = dynamic_cast<StreetProperty*>(prop);
        if (sp != nullptr && sp->canBuild()) {
            eligible_props.push_back(sp);
        }
    }

    if (eligible_props.empty()) {
        std::cout << "Tidak ada properti yang dapat diupgrade saat ini (pastikan sudah monopoli dan memenuhi syarat pemerataan).\n";
        return false;
    }

    // GUI mode: show upgrade panel (only for human players)
    if (!this->isBot() && ctx.gameEngine.getPanelManager()) {
        ctx.gameEngine.getPanelManager()->showUpgrade(*this, ctx);
        return true;
    }

    // Bot players skip CLI upgrade
    if (this->isBot()) {
        return false;
    }

    std::cout << "Properti yang dapat di-upgrade: \n";
    int ctr = 1;
    for (StreetProperty* sprop : eligible_props) {
        std::cout << ctr << ". " << sprop->getCode() << " (" << sprop->getName() << ") - Level: " << sprop->getBuildingCount() << "\n";
        ctr++;
    }

    std::cout << "Masukkan kode properti yang ingin diupgrade: ";
    std::string cd;

    while (true) {
        std::cin >> cd;
        bool found = false;

        for (StreetProperty* sprop : eligible_props) {
            if (cd == sprop->getCode()) {
                found = true;
                try {
                    if (sprop->getBuildingCount() < 4) {
                        sprop->buildHouse();
                        std::cout << "Berhasil membangun rumah di " << sprop->getName() << "\nHarga sewa saat ini: M" << sprop->getRent(ctx) << "\n";
                    } else {
                        sprop->upgradeToHotel();
                        std::cout << "Berhasil membangun hotel di " << sprop->getName() << "\nHarga sewa saat ini: M" << sprop->getRent(ctx) << "\n";
                    }
                    return true;
                } catch (const InsufficientFundsException& e) {
                    std::cout << "Gagal: Saldo tidak cukup!\n";
                    return false;
                }
            }
        }

        if (!found) {
            std::cout << "Properti tidak valid. Silakan masukkan kode properti lain: ";
        }
    }
}

void Player::sell(Property& p) { 
    // double check exception
    if (this->getPropertiesAmount() < 1) {
        // throw EmptyProperties
    }
    //tambahin cash
    this->removeProperty(p);
}

void Player::mortgage(Property* p) {
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot mortgage null property");
    }
    if (p->getOwner() != this) {
        throw InvalidGameStateException("Cannot mortgage property not owned by player");
    }

    const auto it = std::find(this->properties.begin(), this->properties.end(), p);
    if (it == this->properties.end()) {
        throw InvalidGameStateException("Cannot mortgage property not owned by player");
    }

    if (p->getStatus() == PropertyStatus::MORTGAGED) {
        throw InvalidGameStateException("Property is already mortgaged");
    }

    if (p->getStatus() != PropertyStatus::OWNED) {
        throw InvalidGameStateException("Can only mortgage owned properties");
    }

    p->mortgage();
    this->addCash(p->getMortgageValue());
}

// ni nnti make sure parameter type
void Player::addProperty(Property* p, TurnContext& ctx) {
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot add null property");
    }
    if (dynamic_cast<RailroadProperty*> (p) != nullptr){
        (dynamic_cast<RailroadProperty*> (p))->setOwnedRailroadCounter(
            [](const Player* owner) -> int {
            if (owner == nullptr) return 0;
            
            int count = 0;
            // Asumsi Player memiliki method getProperties() yang me-return vector<Property*>
            for (Property* p : owner->getProperties()) {
                // Cek apakah properti 'p' ini adalah sebuah stasiun
                if (dynamic_cast<RailroadProperty*>(p) != nullptr) {
                    count++;
                }
            }
            return count;
        });
    }
    if (dynamic_cast<UtilityProperty*> (p) != nullptr){
        (dynamic_cast<UtilityProperty*> (p))->setOwnedUtilityCounter(
            [](const Player* owner) -> int {
            if (owner == nullptr) return 0;
            
            int count = 0;
            // Asumsi Player memiliki method getProperties() yang me-return vector<Property*>
            for (Property* p : owner->getProperties()) {
                // Cek apakah properti 'p' ini adalah sebuah stasiun
                if (dynamic_cast<UtilityProperty*>(p) != nullptr) {
                    count++;
                }
            }
            return count;
        });
    }
    p->setOwner(this, ctx);
    this->properties.push_back(p);
}

// make sure lgi type p
void Player::removeProperty(Property& p) {
    auto it = std::find_if(
        this->properties.begin(),
        this->properties.end(),
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
    std::cout << "[CARD] " << card->getDescription() << " ditambahkan ke tangan " << getUsername() << ".\n";
}

void Player::discardSCard(int idx) {
    if (idx < 0) {
        throw InvalidGameStateException("Card index cannot be negative, got: " +
                                        std::to_string(idx));
    }
    if (idx >= static_cast<int>(this->hand.size())) {
        throw InvalidGameStateException(
            "Card index out of range: " + std::to_string(idx) +
            ", hand size: " + std::to_string(this->hand.size()));
    }
    this->hand.erase(this->hand.begin() + idx);
}

void Player::useSCard(int idx, TurnContext& ctx) {
    if (this->hand.empty()) {
        throw InvalidGameStateException("No skill card available in hand.");
    }
    if (idx <= 0) {
        throw InvalidGameStateException("Card index must start from 1, got: " + std::to_string(idx));
    }
    if (idx > static_cast<int>(this->hand.size())) {
        throw InvalidGameStateException("Card index out of range: " + std::to_string(idx) + ", hand size: " + std::to_string(this->hand.size()));
    }
    if (this->usedSkillThisTurn) {
        throw InvalidGameStateException("Skill card already used this turn");
    }

    const int internalIdx = idx - 1;
    SkillCard* selectedCard = this->hand[static_cast<size_t>(internalIdx)];
    if (selectedCard == nullptr) {
        throw InvalidGameStateException("Selected skill card is invalid.");
    }

    selectedCard->apply(ctx);
    this->hand.erase(this->hand.begin() + internalIdx);
    this->usedSkillThisTurn = true;
}

void Player::setDiscountRate(int discount) {
    if (discount < 0) {
        throw InvalidGameStateException(
            "Discount rate cannot be negative, got: " +
            std::to_string(discount));
    }
    if (discount > 100) {
        throw InvalidGameStateException(
            "Discount rate cannot exceed 100%, got: " +
            std::to_string(discount));
    }
    this->discountRate = discount;
}

void Player::activateShield() {
    if (this->hasShield) {
        throw InvalidGameStateException("Shield is already active for player " + this->username);
    }
    this->hasShield = true;
}

void Player::deactivateShield() {
    if (!this->hasShield) {
        throw InvalidGameStateException("Shield is already inactive for player " + this->username);
    }
    this->hasShield = false;
}

void Player::enterJail() {
    this->status = PlayerStatus::JAILED;
    this->jailTurns = 3; // artinya masih ada 3 turn lagi di jail
}

void Player::exitJail() {
    this->status = PlayerStatus::ACTIVE;
    this->jailTurns = 0;
}

void Player::decideAction(TurnContext& ctx) {}

bool Player::isBot() const {
    return false;
}

void Player::showHands() {
    // std::cout << "=== Kartu di Tangan ===" << std::endl;
    if (hand.empty()) {
        std::cout << "Tidak ada kartu." << std::endl;
        return;
    }

    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << "   " << (i + 1) << ". " << hand[i]->getDescription() << std::endl;
    }
}

void Player::profile() {
    std::cout << "=== Profil Pemain ===" << std::endl;
    std::cout << "Nama: " << username << std::endl;
    std::cout << "Uang: M" << balance << std::endl;
    std::cout << "Posisi: " << position << std::endl;
    std::cout << "Status: "
              << (status == PlayerStatus::ACTIVE   ? "Aktif"
                  : status == PlayerStatus::JAILED ? "Dipenjara"
                                                   : "Bangkrut")
              << std::endl;
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
        std::cout << (i + 1) << ". " << prop->getName() << " ("
                  << prop->getCode() << ") - ";

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
        throw InvalidGameStateException(
            "Cannot add non-positive cash amount: " + std::to_string(amount));
    }
    this->balance += amount;
}

void Player::deductCash(int amount) {
    if (amount <= 0) {
        throw InvalidGameStateException(
            "Cannot deduct non-positive cash amount: " +
            std::to_string(amount));
    }
    if (this->balance < amount) {
        throw InsufficientFundsException("Insufficient funds for payment of " +
                                         std::to_string(amount));
    }
    this->balance -= amount;
}

void Player::setBankruptStatus() {
    if (this->status == PlayerStatus::BANKRUPT) {
        throw InvalidGameStateException("Player " + this->username +
                                        " is already declared bankrupt");
    }
    this->status = PlayerStatus::BANKRUPT;
}

bool Player::checkBankruptcy(int requiredAmount) {
    if (requiredAmount <= 0) {
        throw InvalidGameStateException(
            "Required amount must be positive, got: " +
            std::to_string(requiredAmount));
    }

    if (this->status == PlayerStatus::BANKRUPT) {
        throw InvalidGameStateException("Player " + this->username +
                                        " is already bankrupt");
    }

    if (this->balance >= requiredAmount) {
        return false;
    }

    int liquidationFunds = 0;
    for (Property* property : this->properties) {
        if (property != nullptr && property->getStatus() == PropertyStatus::OWNED) {
            liquidationFunds += property->getMortgageValue();
        }
    }

    if (this->balance + liquidationFunds < requiredAmount) {
        this->setBankruptStatus();
        return true;
    }

    return false;
}

// Jangan ubah deductCash, biarkan seperti aslinya. Tambahkan fungsi baru ini:

bool Player::payDebt(int amount, TurnContext& ctx) {
    if (amount <= 0) return true;

    if (this->status == PlayerStatus::BANKRUPT) {
        return false;
    }

    // 1. Jika uang cukup, langsung potong saja tanpa repot
    if (this->balance >= amount) {
        this->deductCash(amount);
        return true; 
    }

    // 2. Uang tidak cukup, panggil kurator (BankruptcyManager)
    std::cout << "[PENGUMUMAN] Uang " << this->username << " tidak cukup! Memasuki proses likuidasi...\n";

    if (this->isBot()) {
        try {
            BankruptcyManager::performForcedLiquidation(ctx, amount);
            std::cout << "[BOT] Likuidasi otomatis selesai.\n";
        } catch (const BankruptcyException& e) {
            std::cout << "[BOT] " << e.what() << "\n";
            return false; // Gagal bayar (Bangkrut)
        }
    } else {
        bool survived = BankruptcyManager::resolveDebtByLiquidation(ctx, amount);
        if (!survived) {
            std::cout << "[INFO] " << this->username << " resmi Dinyatakan BANGKRUT!\n";
            return false; // Gagal bayar (Bangkrut)
        }
    }

    if (this->balance < amount) {
        return false;
    }

    // 3. Setelah likuidasi, seharusnya uang sudah cukup. Baru kita potong.
    this->deductCash(amount);
    return true; // Berhasil bayar dan selamat dari kebangkrutan
}

void Player::decrementjailTurns(){
    if (this->jailTurns <= 0){
        throw InvalidGameStateException("Player has invalid jail turns");
    }
    this->jailTurns--;
}


void Player::payRent(Property* p, TurnContext& ctx){
    if (p == nullptr) {
        throw InvalidGameStateException("Cannot pay rent for null property");
    }

    Player* pemilik = p->getOwner();
    if (pemilik == nullptr) {
        throw InvalidGameStateException("Cannot pay rent to null property owner");
    }

    int tagihan = p->getRent(ctx);

    if (tagihan <= 0) {
        return;
    }

    // Gunakan payDebt. Jika return true (berhasil bayar), berikan uangnya ke pemilik
    if (this->payDebt(tagihan, ctx)) {
        pemilik->addCash(tagihan);
        std::cout << "Berhasil membayar sewa sebesar M" << tagihan << " kepada " << pemilik->getUsername() << "\n";
    } else {
        // Jika return false, artinya pemain bangkrut. 
        // Proses penyerahan aset sisa ke pemilik (jika ada aturannya) bisa diletakkan di sini.
    }
}