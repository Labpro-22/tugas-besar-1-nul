#include "core/Dice.hpp"

bool Dice::isDouble() const { return this->die1 == this->die2; }

int Dice::getTotal() const { return this->die1 + this->die2; }

void Dice::roll() {
    this->die1 = (std::rand() % 6) + 1;
    this->die2 = (std::rand() % 6) + 1;

    if (this->isDouble()) this->doubleCount++;
    else this->doubleCount = 0;
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