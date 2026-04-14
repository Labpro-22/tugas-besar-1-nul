#include "card.hpp"
Card::Card(std::string desc) : description(std::move(desc)) {}

std::string Card::getDescription() const {
    return description;
}

