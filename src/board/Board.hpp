#ifndef BOARD_HPP
#define BOARD_HPP


#pragma once
#include "../tile/Tile.hpp"
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// =================================== dummy classes =====================================
class TileConfig{
    //dummy data
};

class StreetTile{
    //dummy data
};

// ======================================================================================



class Board{
    private:
        vector<Tile*> tiles; // 20 hingga 60 tiles
        map<string, int> codeToIndex;
        int size;
    
    public:
        Tile* getTile(int idx);
        Tile* getTileByCode(string cd);
        int getSize();
        vector<StreetTile*> getColorGroup(string clr);
        void buildFromConfig(vector<TileConfig*>);

};

#endif