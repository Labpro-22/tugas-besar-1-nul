#pragma once

class Player;

class BankruptcyManager {
public:
    void check(Player& player, int debt, Player* creditor = nullptr);

    void liquidate(Player* player, int debt);

    void declare(Player* player, Player* creditor);
};