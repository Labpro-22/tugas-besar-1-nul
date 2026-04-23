#pragma once

#include <cstdlib>
#include <ctime>

class Dice {
private:
    int die1;
    int die2;
    int doubleCount;
    bool canRoll;

public:
    explicit Dice() : die1(0), die2(0), doubleCount(0), canRoll(true) {};
    ~Dice() = default;

    int getDie1() const;
    int getDie2() const;

    bool isDouble() const;
    int getTotal() const;

    void roll();
    void setManual(int x, int y);

    void reset();
};