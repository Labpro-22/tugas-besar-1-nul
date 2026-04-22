#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

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
