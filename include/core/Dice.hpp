#pragma once

#include <iostream>

class Dice {
private:
    int die1;
    int die2;
    int doubleCount;

public:
    bool isDouble() const;
    int getTotal() const;

    std::pair<int, int> roll();
    void setManual(int x, int y);
};