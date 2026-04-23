#include "board/Board.hpp"
#include "tile/Tile.hpp"
#include "tile/PropertyTile.hpp"

#include <cctype>

using namespace std;

Board::Board(const map<string, int>& data, int s) : 
    tiles(static_cast<size_t>(s < 0 ? 0 : s)), codeToIndex(data), size(s < 0 ? 0 :s){}

Board::Board(int s) : tiles(static_cast<size_t>(s < 0 ? 0 : s)), size(s < 0 ? 0 : s){
    codeToIndex["initializer"] = -1; //untuk inisialisasi codeToIndex
}

Tile* Board::getTile(int idx){
    // yay ada implementasi STL
    // it akan menyimpan alamat memori lokasi file 
    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [idx](const unique_ptr<Tile>& tile) { 
            return tile != nullptr && tile->getIndex() == idx; 
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
            return tile != nullptr && tile->getCode() == cd; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return it->get(); 
    }
    
    return nullptr; //null jika tidak ditemukan
};

int Board::getSize() const { return size; };
int& Board::getSizeRef() { return size; };

vector<StreetTile*> Board::getColorGroup(string clr){
    vector<StreetTile*> result;

    string normalizedTarget = clr;
    transform(normalizedTarget.begin(), normalizedTarget.end(), normalizedTarget.begin(),
        [](unsigned char ch) { return static_cast<char>(toupper(ch)); });

    for (const unique_ptr<Tile>& tile : tiles) {
        if (tile == nullptr) {
            continue;
        }

        StreetTile* streetTile = dynamic_cast<StreetTile*>(tile.get());
        if (streetTile == nullptr) {
            continue;
        }

        StreetProperty* streetProp = dynamic_cast<StreetProperty*>(streetTile->getProperty());
        if (streetProp == nullptr) {
            continue;
        }

        string tileColor = streetProp->getColorGroup();
        transform(tileColor.begin(), tileColor.end(), tileColor.begin(),
            [](unsigned char ch) { return static_cast<char>(toupper(ch)); });

        if (tileColor == normalizedTarget) {
            result.push_back(streetTile);
        }
    }

    return result;
}

void Board::buildFromConfig(vector<TileConfig*> data){
    (void) data;
    // implement setelah tileconfig selesai
}

Board::~Board() = default;