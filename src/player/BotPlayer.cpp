#include "player/BotPlayer.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

BotPlayer::BotPlayer(std::string name, int balance, int difficulty)
    : Player(name, balance), botDifficulty(difficulty), botName(name) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

BotPlayer::~BotPlayer() {}

bool BotPlayer::isBot() const {
    return true;
}

int BotPlayer::getDifficulty() const {
    return botDifficulty;
}

void BotPlayer::decideAction(TurnContext& ctx) {
    // Bot automatically rolls dice
    displayBotAction("Sedang berpikir...");
}

bool BotPlayer::decideBuyProperty(int price, int currentBalance) {
    // Bot logic for buying property
    int threshold = 100; // Minimum balance to keep after buying
    
    switch (botDifficulty) {
        case 1: // Easy - Buy if can afford with lots of buffer
            threshold = 300;
            break;
        case 2: // Medium - Balanced approach
            threshold = 150;
            break;
        case 3: // Hard - Aggressive buying
            threshold = 50;
            break;
        default:
            threshold = 150;
    }
    
    bool decision = (currentBalance - price) >= threshold;
    
    if (decision) {
        displayBotAction("Memutuskan untuk MEMBELI properti ini!");
    } else {
        displayBotAction("Memutuskan untuk TIDAK membeli properti ini.");
    }
    
    return decision;
}

int BotPlayer::decideBid(int currentBid, int propertyValue, int currentBalance) {
    // Bot logic for auction bidding
    int maxBid = 0;
    
    switch (botDifficulty) {
        case 1: // Easy - Bid low
            maxBid = propertyValue * 0.6;
            break;
        case 2: // Medium - Bid up to property value
            maxBid = propertyValue;
            break;
        case 3: // Hard - Bid aggressively
            maxBid = propertyValue * 1.3;
            break;
        default:
            maxBid = propertyValue;
    }
    
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
    // Bot logic for building houses
    int threshold = 0;
    
    switch (botDifficulty) {
        case 1: // Easy - Build only with lots of money
            threshold = 500;
            break;
        case 2: // Medium - Build with moderate money
            threshold = 300;
            break;
        case 3: // Hard - Build aggressively
            threshold = 150;
            break;
        default:
            threshold = 300;
    }
    
    bool decision = (currentBalance - buildCost) >= threshold;
    
    if (decision) {
        displayBotAction("Memutuskan untuk membangun rumah!");
    }
    
    return decision;
}

bool BotPlayer::decideMortgage(int mortgageValue) {
    // Bot usually mortgages when in need of money
    // This is called during bankruptcy/need cash situations
    displayBotAction("Menggadaikan properti untuk mendapatkan M" + std::to_string(mortgageValue));
    return true;
}

bool BotPlayer::decideRedeem(int redeemCost, int currentBalance) {
    // Bot logic for redeeming mortgaged property
    int threshold = 200;
    
    switch (botDifficulty) {
        case 1: // Easy - Only redeem with lots of money
            threshold = 500;
            break;
        case 2: // Medium - Balanced
            threshold = 300;
            break;
        case 3: // Hard - Redeem quickly
            threshold = 150;
            break;
        default:
            threshold = 300;
    }
    
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
    
    // Bot uses skill card randomly based on difficulty
    int useChance = 0;
    
    switch (botDifficulty) {
        case 1: // Easy - 30% chance to use card
            useChance = 30;
            break;
        case 2: // Medium - 50% chance to use card
            useChance = 50;
            break;
        case 3: // Hard - 70% chance to use card
            useChance = 70;
            break;
        default:
            useChance = 50;
    }
    
    int roll = std::rand() % 100;
    if (roll < useChance) {
        int cardIndex = std::rand() % cardCount;
        displayBotAction("Menggunakan kartu kemampuan!");
        return cardIndex;
    }
    
    return -1; // Don't use any card
}

int BotPlayer::decideJailAction(int jailFine, int currentBalance, bool hasJailCard) {
    // Bot logic for jail decision
    // Returns: 1 = Pay fine, 2 = Use card, 3 = Try roll
    
    // Always use jail card if available
    if (hasJailCard) {
        displayBotAction("Menggunakan kartu Bebas Penjara!");
        return 2;
    }
    
    // Decide based on difficulty and money
    switch (botDifficulty) {
        case 1: // Easy - Try to roll (save money)
            if (currentBalance < jailFine + 300) {
                displayBotAction("Mencoba melempar dadu untuk bebas...");
                return 3;
            }
            break;
        case 2: // Medium - Balanced
            if (currentBalance < jailFine + 500) {
                displayBotAction("Mencoba melempar dadu untuk bebas...");
                return 3;
            }
            break;
        case 3: // Hard - Pay to get out quickly
            if (currentBalance >= jailFine + 200) {
                displayBotAction("Membayar denda M" + std::to_string(jailFine) + " untuk bebas cepat!");
                return 1;
            }
            break;
    }
    
    // Default: try to roll
    displayBotAction("Mencoba melempar dadu untuk bebas...");
    return 3;
}

void BotPlayer::displayBotAction(const std::string& action) {
    std::cout << "[BOT " << botName << "] " << action << std::endl;
}
