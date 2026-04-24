#include "core/Dice.hpp"
#include <iostream>
#include <random>

bool Dice::isDouble() const {
    return this->die1 == this->die2;
}

int Dice::getDie1() const {
    return this->die1;
}

int Dice::getDie2() const {
    return this->die2;
}

int Dice::getTotal() const {
    return this->die1 + this->die2;
}

bool Dice::roll() {
    if (!canRoll)
        return false;

    if (this->doubleCount > 2) {
        canRoll = false;
        // go jail
        return true;
    }

    static std::mt19937 rng(
        std::random_device{}()); // seeded once automatically
    std::uniform_int_distribution<int> dist(1, 6);

    this->die1 = dist(rng);
    this->die2 = dist(rng);

    if (this->isDouble()) {
        this->doubleCount++;
    } else
        canRoll = false;
    return true;
}

void Dice::setManual(int x, int y) {
    this->die1 = x;
    this->die2 = y;

    if (this->isDouble()) {
        this->doubleCount++;
    } else {
        this->doubleCount = 0;
    }
}

void Dice::reset() {
    this->setManual(0, 0);
    this->doubleCount = 0;
    this->canRoll = true;
}