#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
// using namespace std;

#include "tile/Tile.hpp"
#include "property/Property.hpp"

// Forward declarations
class TileConfig;
class StreetTile;

class Board{
    private:
        std::vector<std::unique_ptr<Tile>> tiles; // 20 hingga 60 tiles; bisa menggunakan smart pointer untuk RAII
        std::vector<std::unique_ptr<Property>> properties; // owner for property instances used by PropertyTile
        std::map<std::string, int> codeToIndex; // masi error nnti cek lgi dah mo turu dl
        int size;
    
    public:
        Board(const std::map<std::string, int>& data, int s);
        Board(int s);

        Board(Board&&) noexcept = default;            // Move Constructor
        Board& operator=(Board&&) noexcept = default; // Move Assignment Operator

        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;
        
        Tile* getTile(int idx);
        Tile* getTileByCode(std::string cd);
        void setTileAt(int idx, std::unique_ptr<Tile> tile);
        int getPlacedTileCount() const;
        std::vector<std::unique_ptr<Tile>>& getAllTiles();
        int getSize() const;
        int& getSizeRef();
        std::vector<StreetTile*> getColorGroup(std::string clr);
        void buildFromConfig(std::vector<TileConfig*> data);

        void generateDefaultBoard(); 

        ~Board();
};
