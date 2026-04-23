#include "core/Dice.hpp"

bool Dice::isDouble() const { return this->die1 == this->die2; }

int Dice::getDie1() const { return this->die1; }

int Dice::getDie2() const { return this->die2; }

int Dice::getTotal() const { return this->die1 + this->die2; }

void Dice::roll() {
    if (!canRoll) return;

    if (this->doubleCount > 2) {
        canRoll = false;
        // go jail
    }
    this->die1 = (std::rand() % 6) + 1;
    this->die2 = (std::rand() % 6) + 1;

    if (this->isDouble()) {
        this->doubleCount++;
    }
    else canRoll = false;
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