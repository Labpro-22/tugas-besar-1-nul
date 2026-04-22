#ifndef TILE_HPP
#define TILE_HPP

#pragma once

#include <iostream>
#include <vector>
using namespace std;

// Forward declarations
class Board;
class Player;
class TurnContext;

class Tile{
    protected:
        int index;
        string code;
        string name;
        string codeCategory;

    public:
        Tile(int idx, string cd, string nm, string cat);
        int getIndex();
        string getCode();
        string getName();
        string getDisplay();
        string getColorTag();
        virtual void onLanded(Player* player, TurnContext& ctx);
};

#endif // TILE_HPP
