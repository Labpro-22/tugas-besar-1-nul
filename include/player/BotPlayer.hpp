#pragma once

#include "Player.hpp"
#include <string>

/**
 * BotPlayer - Computer-controlled player for Nimonspoli
 * Inherits from Player and implements automatic decision making
 */
class BotPlayer : public Player {
  private:
    int botDifficulty; // 1 = Easy, 2 = Medium, 3 = Hard
    std::string botName;

  public:
    // Constructor
    BotPlayer(std::string name, int balance, int difficulty = 1);

    // Destructor
    ~BotPlayer();

    // Check if this is a bot player
    bool isBot() const override;

    // Get bot difficulty level
    int getDifficulty() const;

    // Main decision making method - called when it's bot's turn
    void decideAction(TurnContext& ctx) override;

    // Get bot's decision to buy a property
    bool decideBuyProperty(int price, int currentBalance);

    // Get bot's decision for auction bidding
    int decideBid(int currentBid, int propertyValue, int currentBalance);

    // Get bot's decision to build houses
    bool decideBuildHouse(int buildCost, int currentBalance);

    // Get bot's decision to mortgage property
    bool decideMortgage(int mortgageValue);

    // Get bot's decision to redeem mortgaged property
    bool decideRedeem(int redeemCost, int currentBalance);

    // Get bot's decision to use a skill card
    int decideUseSkillCard(int cardCount);

    // Get bot's decision in jail
    int decideJailAction(int jailFine, int currentBalance, bool hasJailCard);

    // Display bot action
    void displayBotAction(const std::string& action);
};
