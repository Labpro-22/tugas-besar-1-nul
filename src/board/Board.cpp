#include "board/Board.hpp"
#include "core/ConfigLoader.hpp"
#include "tile/ActionTile.hpp"
#include "tile/PropertyTile.hpp"
#include "tile/Tile.hpp"

#include "config/ActionTileData.hpp"
#include "config/Config.hpp"
#include "config/PropertyData.hpp"
#include "property/RailroadProperty.hpp"
#include "property/StreetProperty.hpp"
#include "property/UtilityProperty.hpp"

#include "exception/ConfigException.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace {
std::string toUpperAscii(const std::string& input) {
    std::string result = input;
    std::transform(
        result.begin(), result.end(), result.begin(), [](unsigned char ch) {
            return static_cast<char>(std::toupper(ch));
        });
    return result;
}
} // namespace

Board::Board(const map<string, int>& data, int s)
    : tiles(static_cast<size_t>(s < 0 ? 0 : s)), codeToIndex(data),
      size(s < 0 ? 0 : s) {}

Board::Board(int s)
    : tiles(static_cast<size_t>(s < 0 ? 0 : s)), size(s < 0 ? 0 : s) {
    codeToIndex["initializer"] = -1; // untuk inisialisasi codeToIndex
}

Board::Board(Board&& other) noexcept
    : tiles(std::move(other.tiles)),
      properties(std::move(other.properties)),
      codeToIndex(std::move(other.codeToIndex)),
      size(other.size) {
    other.tiles.clear();
    other.properties.clear();
    other.codeToIndex.clear();
    other.size = 0;
}

Board& Board::operator=(Board&& other) noexcept {
    if (this != &other) {
        clearOwnedData();

        tiles = std::move(other.tiles);
        properties = std::move(other.properties);
        codeToIndex = std::move(other.codeToIndex);
        size = other.size;

        other.tiles.clear();
        other.properties.clear();
        other.codeToIndex.clear();
        other.size = 0;
    }
    return *this;
}

void Board::clearOwnedData() {
    for (Tile* tile : tiles) {
        delete tile;
    }
    tiles.clear();

    for (Property* property : properties) {
        delete property;
    }
    properties.clear();
}

std::vector<Property*> Board::getAllProperties(){
    return properties;
}

const std::vector<Property*> Board::getAllProperties() const {
    return properties;
}

Tile* Board::getTile(int idx){
    if (idx < 0 || idx >= size) {
        return nullptr;
    }

    return tiles[static_cast<size_t>(idx)];
};

const Tile* Board::getTile(int idx) const {
    if (idx < 0 || idx >= size) {
        return nullptr;
    }

    return tiles[static_cast<size_t>(idx)];
};

Tile* Board::getTileByCode(string cd) {
    auto mapped = codeToIndex.find(cd);
    if (mapped != codeToIndex.end()) {
        return getTile(mapped->second);
    }

    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [cd](Tile* tile) {
            return tile != nullptr && tile->getCode() == cd; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return *it;
    }

    return nullptr; // null jika tidak ditemukan
}

const Tile* Board::getTileByCode(string cd) const {
    auto mapped = codeToIndex.find(cd);
    if (mapped != codeToIndex.end()) {
        return getTile(mapped->second);
    }

    auto it = find_if(tiles.begin(), tiles.end(), 
        // fungsi lambda
        [cd](Tile* tile) {
            return tile != nullptr && tile->getCode() == cd; 
        }
    );

    // jika pencarian berhasil
    if (it != tiles.end()) {
        return *it;
    }

    return nullptr; // null jika tidak ditemukan
}

std::vector<Tile*>& Board::getAllTiles(){
    return tiles;
}

void Board::setTileAt(int idx, Tile* tile) {
    if (idx < 0 || idx >= size) {
        delete tile;
        return;
    }
    if (tile == nullptr) {
        return;
    }

    Tile*& existing = tiles[static_cast<size_t>(idx)];
    if (existing != nullptr) {
        codeToIndex.erase(existing->getCode());
        delete existing;
    }

    codeToIndex[tile->getCode()] = idx;
    existing = tile;
}

int Board::getPlacedTileCount() const {
    int count = 0;
    for (Tile* tile : tiles) {
        if (tile != nullptr) {
            ++count;
        }
    }
    return count;
}

int Board::getSize() const {
    return size;
};
int& Board::getSizeRef() {
    return size;
};

vector<StreetTile*> Board::getColorGroup(string clr) {
    vector<StreetTile*> result;

    string normalizedTarget = clr;
    transform(normalizedTarget.begin(),
              normalizedTarget.end(),
              normalizedTarget.begin(),
              [](unsigned char ch) { return static_cast<char>(toupper(ch)); });

    for (Tile* tile : tiles) {
        if (tile == nullptr) {
            continue;
        }

        StreetTile* streetTile = dynamic_cast<StreetTile*>(tile);
        if (streetTile == nullptr) {
            continue;
        }

        StreetProperty* streetProp =
            dynamic_cast<StreetProperty*>(streetTile->getProperty());
        if (streetProp == nullptr) {
            continue;
        }

        string tileColor = streetProp->getColorGroup();
        transform(
            tileColor.begin(),
            tileColor.end(),
            tileColor.begin(),
            [](unsigned char ch) { return static_cast<char>(toupper(ch)); });

        if (tileColor == normalizedTarget) {
            result.push_back(streetTile);
        }
    }

    return result;
}

void Board::buildFromConfig(const Config& config) {
    if (size < 40) {
        size = 40;
    }

    clearOwnedData();
    tiles.resize(static_cast<size_t>(size));
    codeToIndex.clear();

    auto normalizeName = [](std::string text) {
        std::replace(text.begin(), text.end(), '_', ' ');
        return text;
    };

    auto normalizeColor = [](std::string color) {
        std::replace(color.begin(), color.end(), '_', ' ');
        return color;
    };

    auto addStreet = [this](int idx,
                            const std::string& code,
                            const std::string& name,
                            int buyPrice,
                            int mortgageValue,
                            const std::string& color,
                            int housePrice,
                            int hotelPrice,
                            const std::vector<int>& rentTable) {
        auto* prop = new StreetProperty(
            code, name, buyPrice, mortgageValue, color, housePrice, hotelPrice, rentTable);
        properties.push_back(prop);
        setTileAt(idx, new StreetTile(idx, *prop));
    };

    auto addRailroad = [this, &config](int idx,
                                       const std::string& code,
                                       const std::string& name,
                                       int buyPrice,
                                       int mortgageValue) {
        auto* prop = new RailroadProperty(
            code, name, buyPrice, mortgageValue, config.railroad.rentTable);
        properties.push_back(prop);
        setTileAt(idx, new RailroadTile(idx, *prop));
    };

    auto addUtility = [this, &config](int idx,
                                      const std::string& code,
                                      const std::string& name,
                                      int buyPrice,
                                      int mortgageValue) {
        auto* prop = new UtilityProperty(
            code, name, buyPrice, mortgageValue, config.utility.multiplierTable);
        properties.push_back(prop);
        setTileAt(idx, new UtilityTile(idx, *prop));
    };

    for (const PropertyData& property : config.properties) {
        const int idx = property.id - 1;
        if (idx < 0 || idx >= size) {
            continue;
        }

        const std::string name = normalizeName(property.name);
        const std::string type = toUpperAscii(property.type);

        if (type == "STREET") {
            addStreet(idx,
                      property.code,
                      name,
                      property.buyPrice,
                      property.mortgageValue,
                      normalizeColor(property.color),
                      property.houseUpgrade,
                      property.hotelUpgrade,
                      property.rentTable);
        } else if (type == "RAILROAD") {
            addRailroad(idx,
                        property.code,
                        name,
                        property.buyPrice,
                        property.mortgageValue);
        } else if (type == "UTILITY") {
            addUtility(idx,
                       property.code,
                       name,
                       property.buyPrice,
                       property.mortgageValue);
        }
    }

    for (const ActionTileData& action : config.actionTiles) {
        const int idx = action.id - 1;
        if (idx < 0 || idx >= size) {
            continue;
        }

        const std::string actionName = normalizeName(action.name);

        if (action.isFestival()) {
            setTileAt(idx, new FestivalTile(idx, action.code, actionName));
        } else if (action.isTax()) {
            const TaxType taxType = (action.code == "PPH") ? TaxType::PPH : TaxType::PBM;
            setTileAt(idx, new TaxTile(idx, action.code, actionName, taxType));
        } else if (action.isChanceCard() || action.isCommunityChest()) {
            setTileAt(idx, new CardTile(idx, action.code, actionName));
        } else if (action.isSpecial()) {
            if (action.code == "GO") {
                setTileAt(idx, new GoTile(idx, action.code, actionName));
            } else if (action.code == "PEN") {
                setTileAt(idx, new JailTile(idx, action.code, actionName));
            } else if (action.code == "BBP") {
                setTileAt(idx, new FreeParkingTile(idx, action.code, actionName));
            } else if (action.code == "PPJ") {
                setTileAt(idx, new GoToJailTile(idx, action.code, actionName));
            } else {
                setTileAt(idx, new ActionTile(idx, action.code, actionName));
            }
        }
    }
}

void Board::generateDefaultBoard() {
    // Regenerate board from scratch to ensure stable indices and code mapping.
    if (size < 40) {
        size = 40;
    }

    clearOwnedData();
    tiles.resize(static_cast<size_t>(size));
    codeToIndex.clear();

    auto addStreet = [this](int idx,
                            const string& code,
                            const string& name,
                            int buyPrice,
                            int mortgageValue,
                            const string& color,
                            int housePrice,
                            int hotelPrice,
                            const vector<int>& rentTable) {
        auto* prop = new StreetProperty(
            code, name, buyPrice, mortgageValue, color, housePrice, hotelPrice, rentTable);
        properties.push_back(prop);
        setTileAt(idx, new StreetTile(idx, *prop));
    };

    auto addRailroad = [this](int idx,
                              const string& code,
                              const string& name,
                              int buyPrice,
                              int mortgageValue,
                              const map<int, int>& rentTable) {
        auto* prop = new RailroadProperty(code, name, buyPrice, mortgageValue, rentTable);
        properties.push_back(prop);
        setTileAt(idx, new RailroadTile(idx, *prop));
    };

    auto addUtility = [this](int idx,
                             const string& code,
                             const string& name,
                             int buyPrice,
                             int mortgageValue,
                             const map<int, int>& multTable) {
        auto* prop = new UtilityProperty(code, name, buyPrice, mortgageValue, multTable);
        properties.push_back(prop);
        setTileAt(idx, new UtilityTile(idx, *prop));
    };

    setTileAt(0, new GoTile(0, "GO", "Petak Mulai"));
    addStreet(1, "GRT", "Garut", 60, 30, "Coklat", 50, 50, {2, 10, 30, 90, 160, 250});
    setTileAt(2, new CardTile(2, "DNU", "Dana Umum"));
    addStreet(3, "TSK", "Tasikmalaya", 60, 30, "Coklat", 50, 50, {4, 20, 60, 180, 320, 450});
    setTileAt(4, new TaxTile(4, "PPH", "Pajak Penghasilan", TaxType::PPH));
    addRailroad(5, "GBR", "Stasiun Gambir", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(6, "BGR", "Bogor", 100, 50, "Biru Muda", 50, 50, {6, 30, 90, 270, 400, 550});
    setTileAt(7, new FestivalTile(7, "FES", "Festival"));
    addStreet(8, "DPK", "Depok", 100, 50, "Biru Muda", 50, 50, {6, 30, 90, 270, 400, 550});
    addStreet(9, "BKS", "Bekasi", 120, 60, "Biru Muda", 50, 50, {8, 40, 100, 300, 450, 600});
    setTileAt(10, new JailTile(10, "PEN", "Penjara"));
    addStreet(11, "MGL", "Magelang", 140, 70, "Merah Muda", 100, 100, {10, 50, 150, 450, 625, 750});
    addUtility(12, "PLN", "PLN", 150, 75, {{1, 4}, {2, 10}});
    addStreet(13, "SOL", "Solo", 140, 70, "Merah Muda", 100, 100, {10, 50, 150, 450, 625, 750});
    addStreet(14, "YOG", "Yogyakarta", 160, 80, "Merah Muda", 100, 100, {12, 60, 180, 500, 700, 900});
    addRailroad(15, "STB", "Stasiun Bandung", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(16, "MAL", "Malang", 180, 90, "Orange", 100, 100, {14, 70, 200, 550, 750, 950});
    setTileAt(17, new CardTile(17, "DNU", "Dana Umum"));
    addStreet(18, "SMG", "Semarang", 180, 90, "Orange", 100, 100, {14, 70, 200, 550, 750, 950});
    addStreet(19, "SBY", "Surabaya", 200, 100, "Orange", 100, 100, {16, 80, 220, 600, 800, 1000});
    setTileAt(20, new FreeParkingTile(20, "BBP", "Bebas Parkir"));
    addStreet(21, "MKS", "Makassar", 220, 110, "Merah", 150, 150, {18, 90, 250, 700, 875, 1050});
    
    setTileAt(22, new CardTile(22, "KSP", "Kesempatan"));
    
    addStreet(23, "BLP", "Balikpapan", 220, 110, "Merah", 150, 150, {18, 90, 250, 700, 875, 1050});
    addStreet(24, "MND", "Manado", 240, 120, "Merah", 150, 150, {20, 100, 300, 750, 925, 1100});
    addRailroad(25, "TUG", "Stasiun Tugu", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(26, "PLB", "Palembang", 260, 130, "Kuning", 150, 150, {22, 110, 330, 800, 975, 1150});
    addStreet(27, "PKB", "Pekanbaru", 260, 130, "Kuning", 150, 150, {22, 110, 330, 800, 975, 1150});
    addUtility(28, "PAM", "PAM", 150, 75, {{1, 4}, {2, 10}});
    addStreet(29, "MED", "Medan", 280, 140, "Kuning", 150, 150, {24, 120, 360, 850, 1025, 1200});
    setTileAt(30, new GoToJailTile(30, "PPJ", "Pergi ke Penjara"));
    addStreet(31, "BDG", "Bandung", 300, 150, "Hijau", 200, 200, {26, 130, 390, 900, 1100, 1275});
    addStreet(32, "DEN", "Denpasar", 300, 150, "Hijau", 200, 200, {26, 130, 390, 900, 1100, 1275});
    setTileAt(33, new FestivalTile(33, "FES", "Festival"));
    addStreet(34, "MTR", "Mataram", 320, 160, "Hijau", 200, 200, {28, 150, 450, 1000, 1200, 1400});
    addRailroad(35, "GUB", "Stasiun Gubeng", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    setTileAt(36, new CardTile(36, "KSP", "Kesempatan"));
    addStreet(37, "JKT", "Jakarta", 350, 175, "Biru Tua", 200, 200, {35, 175, 500, 1100, 1300, 1500});
    setTileAt(38, new TaxTile(38, "PBM", "Pajak Barang Mewah", TaxType::PBM));
    addStreet(39, "IKN", "Ibu Kota Nusantara", 400, 200, "Biru Tua", 200, 200, {50, 200, 600, 1400, 1700, 2000});
}


Board::~Board() {
    clearOwnedData();
}
