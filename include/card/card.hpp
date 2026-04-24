#pragma once

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

class Player;
class TurnContext;

class Card {
  protected:
    std::string description;

  public:
    Card(std::string desc);
    virtual ~Card() = default;

    std::string getDescription() const;

    virtual void execute(Player* player, TurnContext& ctx) = 0;
};
