#include "tile/Tile.hpp"
#include "player/Player.h"
#include "core/TurnContext.hpp"

#include <iostream>
#include <vector>
using namespace std;

Tile::Tile(int idx, string cd, string nm, string cat) : index(idx), code(cd), name(nm), codeCategory(cat){}

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

string Tile::getColorTag(){
    return codeCategory;
};

void Tile::onLanded(Player* player, TurnContext& ctx){
    // Default implementation - do nothing
    // Subclasses will override this
}
