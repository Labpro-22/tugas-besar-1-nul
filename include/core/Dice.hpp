#pragma once

#include <iostream>

class Dice {
private:
    int die1;
    int die2;
    int doubleCount;

public:
    explicit Dice() : die1(0), die2(0), doubleCount(0) {};
    ~Dice() = default;

    bool isDouble() const;
    int getTotal() const;

    void roll();
    void setManual(int x, int y);
};