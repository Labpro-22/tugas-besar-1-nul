#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
// using namespace std;

#include "property/Property.hpp"
#include "tile/Tile.hpp"

// Forward declarations
class TileConfig;
class StreetTile;
class Config;

class Board{
    private:
        std::vector<Tile*> tiles; // owner for all tile instances
        std::vector<Property*> properties; // owner for property instances used by PropertyTile
        std::map<std::string, int> codeToIndex; // masi error nnti cek lgi dah mo turu dl
        int size;

        void clearOwnedData();
    
    public:
        Board(const std::map<std::string, int>& data, int s);
        Board(int s);

        Board(Board&&) noexcept;            // Move Constructor
        Board& operator=(Board&&) noexcept; // Move Assignment Operator

        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;
        
        std::vector<Property*> getAllProperties();
        Tile* getTile(int idx);
        Tile* getTileByCode(std::string cd);
        void setTileAt(int idx, Tile* tile);
        int getPlacedTileCount() const;
        std::vector<Tile*>& getAllTiles();
        int getSize() const;
        int& getSizeRef();
        std::vector<StreetTile*> getColorGroup(std::string clr);
        void buildFromConfig(const Config& config);

        void generateDefaultBoard(); 

        ~Board();
};
