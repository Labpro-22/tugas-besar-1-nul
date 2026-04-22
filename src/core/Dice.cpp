#include "core/Dice.hpp"

Dice::Dice() : die1(1), die2(1), manualMode(false) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Dice::roll() {
    if (!manualMode) {
        die1 = (std::rand() % 6) + 1;
        die2 = (std::rand() % 6) + 1;
    }
    // If manual mode, keep the set values
}

void Dice::setManual(int d1, int d2) {
    if (d1 >= 1 && d1 <= 6 && d2 >= 1 && d2 <= 6) {
        die1 = d1;
        die2 = d2;
        manualMode = true;
    }
}

int Dice::getDie1() const {
    return die1;
}

int Dice::getDie2() const {
    return die2;
}

int Dice::getTotal() const {
    return die1 + die2;
}

bool Dice::isDouble() const {
    return die1 == die2;
}

void Dice::reset() {
    manualMode = false;
}
