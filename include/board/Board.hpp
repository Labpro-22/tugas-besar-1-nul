#pragma once
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
        vector<Tile*> tiles; // 20 hingga 60 tiles; bisa menggunakan smart pointer untuk RAII
        map<string, int> codeToIndex;
        int size;
    
    public:
        Board(const map<string, int>& data, int s);
        Board(int s);
        Tile* getTile(int idx);
        Tile* getTileByCode(string cd);
        int getSize();
        vector<StreetTile*> getColorGroup(string clr);
        void buildFromConfig(vector<TileConfig*> data);

};