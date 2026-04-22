#ifndef TILE_HPP
#define TILE_HPP

#pragma once

#include "board/Board.hpp"
#include <iostream>
#include <vector>
using namespace std;

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

#endif