#include "tile/Tile.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"

#include <iostream>
#include <vector>
using namespace std;


Tile::Tile(int idx, string cd, string nm) : index(idx), code(cd), name(nm){}

int Tile::getIndex(){
    return index;
};

string Tile::getCode(){
    return code;
};

string Tile::getName(){
    return name;
};

string Tile::getDisplay(){
    return code + " - " + name;
};
