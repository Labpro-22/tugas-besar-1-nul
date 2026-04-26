#include "tile/Tile.hpp"
#include "core/TurnContext.hpp"
#include "player/Player.hpp"

#include <iostream>
#include <vector>
using namespace std;

Tile::Tile(int idx, string cd, string nm) : index(idx), code(cd), name(nm) {}

int Tile::getIndex() const {
    return index;
};

string Tile::getCode() const {
    return code;
};

string Tile::getName() const {
    return name;
};

string Tile::getDisplay() const {
    return code + " - " + name;
};
