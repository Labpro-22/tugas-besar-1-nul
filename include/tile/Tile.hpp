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

    public:
        Tile(int idx, string cd, string nm);
        int getIndex();
        string getCode();
        string getName();
        string getDisplay();
        virtual void onLanded(TurnContext& ctx) = 0; //consider hapus player karena ctx sudah ada konteks player saat ini.
};

#endif // TILE_HPP
