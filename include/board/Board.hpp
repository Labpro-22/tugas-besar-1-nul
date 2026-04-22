#ifndef BOARD_HPP
#define BOARD_HPP

#include "tile/Tile.hpp"

#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

class Tile;
class StreetTile;

// =================================== dummy classes =====================================
class TileConfig{
    //dummy data
};

// ======================================================================================



class Board{
    private:
        vector<unique_ptr<Tile>> tiles; // 20 hingga 60 tiles; bisa menggunakan smart pointer untuk RAII
        map<string, int> codeToIndex;
        int size;
    
    public:
        Board(const map<string, int>& data, int s);
        Board(int s);

        Board(Board&&) noexcept = default;            // Move Constructor
        Board& operator=(Board&&) noexcept = default; // Move Assignment Operator

        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;
        
        Tile* getTile(int idx);
        Tile* getTileByCode(string cd);
        int getSize();
        vector<StreetTile*> getColorGroup(string clr);
        void buildFromConfig(vector<TileConfig*> data);

        ~Board();
};

#endif