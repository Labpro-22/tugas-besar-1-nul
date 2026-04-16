#include "../../include/board/Board.hpp"
#include "../../include/tile/Tile.hpp"

using namespace std;

Board::Board(const map<string, int>& data, int s) : 
    tiles(s, nullptr), codeToIndex(data), size(s){
        if (size <0){
            size == 0;
            //can either throw exception or cap size minimum at 0
        }
    };

Board::Board(int s) : tiles(s, nullptr), size(s){
    codeToIndex["initializer"] = -1; //untuk inisialisasi codeToIndex
    if (size <0){
        size == 0;
        //can either throw exception or cap size minimum at 0
    }
};

Tile* Board::getTile(int idx){
    // yay ada implementasi STL
    // it akan menyimpan alamat memori lokasi file 
    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [idx](Tile* tile) { 
            return tile->getIndex() == idx; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return *it; 
    }
    
    return nullptr; //null jika tidak ditemukan
};

Tile* Board::getTileByCode(string cd){
    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [cd](Tile* tile) { 
            return tile->getCode() == cd; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return *it; 
    }
    
    return nullptr; //null jika tidak ditemukan
};

int Board::getSize(){return size;};

vector<StreetTile*> Board::getColorGroup(string clr){
    //implement setelah streettile selesai
};

void Board::buildFromConfig(vector<TileConfig*> data){
    //implement setelah tileconfig selesai
};
