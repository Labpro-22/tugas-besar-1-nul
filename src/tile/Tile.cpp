#include "Tile.hpp"

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
    // get display
};

string Tile::getColorTag(){
    return codeCategory;
};

// virtual void Tile::onLanded(){
//     int x = 67;
//     cout << "Player tiba di Tile " << x << "\n";
// };