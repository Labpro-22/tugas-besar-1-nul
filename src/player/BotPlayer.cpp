#include "player/BotPlayer.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

BotPlayer::BotPlayer(std::string name, int balance)
    : Player(name, balance), botName(name) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

BotPlayer::~BotPlayer() {}

bool BotPlayer::isBot() const {
    return true;
}

void BotPlayer::decideAction(TurnContext& ctx) {
    // Bot automatically rolls dice
    displayBotAction("Sedang berpikir...");
}

bool BotPlayer::decideBuyProperty(int price, int currentBalance) {
    // Medium strategy: Buy if can afford with moderate buffer
    int threshold = 150;

    bool decision = (currentBalance - price) >= threshold;

    if (decision) {
        displayBotAction("Memutuskan untuk MEMBELI properti ini!");
    } else {
        displayBotAction("Memutuskan untuk TIDAK membeli properti ini.");
    }

    return decision;
}

int BotPlayer::decideBid(int currentBid, int propertyValue, int currentBalance) {
    // Medium strategy: Bid up to property value
    int maxBid = propertyValue;

    // Ensure we don't bid more than we can afford (keeping some buffer)
    int affordableBid = currentBalance - 100;
    maxBid = std::min(maxBid, affordableBid);

    if (currentBid < maxBid) {
        int nextBid = currentBid + 10;
        if (nextBid <= maxBid) {
            displayBotAction("Menawar M" + std::to_string(nextBid));
            return nextBid;
        }
    }

    displayBotAction("Melewati lelang (PASS)");
    return -1; // Pass
}

bool BotPlayer::decideBuildHouse(int buildCost, int currentBalance) {
    // Medium strategy: Build with moderate money
    int threshold = 300;

    bool decision = (currentBalance - buildCost) >= threshold;

    if (decision) {
        displayBotAction("Memutuskan untuk membangun rumah!");
    }

    return decision;
}

bool BotPlayer::decideMortgage(int mortgageValue) {
    // Bot usually mortgages when in need of money
    displayBotAction("Menggadaikan properti untuk mendapatkan M" + std::to_string(mortgageValue));
    return true;
}

bool BotPlayer::decideRedeem(int redeemCost, int currentBalance) {
    // Medium strategy: Balanced approach
    int threshold = 300;

    bool decision = (currentBalance - redeemCost) >= threshold;

    if (decision) {
        displayBotAction("Memutuskan untuk menebus properti yang digadaikan.");
    }

    return decision;
}

int BotPlayer::decideUseSkillCard(int cardCount) {
    if (cardCount <= 0) {
        return -1;
    }

    // Medium strategy: 50% chance to use card
    int useChance = 50;

    int roll = std::rand() % 100;
    if (roll < useChance) {
        int cardIndex = std::rand() % cardCount;
        displayBotAction("Menggunakan kartu kemampuan!");
        return cardIndex;
    }

    return -1; // Don't use any card
}

int BotPlayer::decideJailAction(int jailFine,
                                int currentBalance,
                                bool hasJailCard) {
    // Bot logic for jail decision
    // Returns: 1 = Pay fine, 2 = Use card, 3 = Try roll

    // Always use jail card if available
    if (hasJailCard) {
        displayBotAction("Menggunakan kartu Bebas Penjara!");
        return 2;
    }

    // Medium strategy: Balanced approach
    if (currentBalance < jailFine + 500) {
        displayBotAction("Mencoba melempar dadu untuk bebas...");
        return 3;
    }

    // Pay to get out if have enough money
    displayBotAction("Membayar denda M" + std::to_string(jailFine) + " untuk bebas cepat!");
    return 1;
}

void BotPlayer::displayBotAction(const std::string& action) {
    std::cout << "[BOT] " << botName << action << std::endl;
}