#include "../../include/board/Board.hpp"
#include "../../include/tile/Tile.hpp"

using namespace std;

Board::Board(const map<string, int>& data, int s) : 
    tiles(s < 0 ? 0 : s), codeToIndex(data), size(s < 0 ? 0 :s){
            //can either throw exception or cap size minimum at 0
    }

Board::Board(int s) : tiles(s < 0 ? 0 : s), size(s < 0 ? 0 : s){
        //can either throw exception or cap size minimum at 0
}

Tile* Board::getTile(int idx){
    // yay ada implementasi STL
    // it akan menyimpan alamat memori lokasi file 
    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [idx](const unique_ptr<Tile>& tile) { 
            return tile->getIndex() == idx; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return it->get(); 
    }
    
    return nullptr; //null jika tidak ditemukan
};

Tile* Board::getTileByCode(string cd){
    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [cd](const unique_ptr<Tile>& tile) { 
            return tile->getCode() == cd; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return it->get(); 
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
