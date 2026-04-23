#include "board/Board.hpp"
#include "core/ConfigLoader.hpp"
#include "tile/Tile.hpp"
#include "tile/PropertyTile.hpp"
#include "tile/ActionTile.hpp"

#include "config/Config.hpp"
#include "config/PropertyData.hpp"
#include "config/ActionTileData.hpp"
#include "property/StreetProperty.hpp"
#include "property/RailroadProperty.hpp"
#include "property/UtilityProperty.hpp"

#include "exception/ConfigException.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace {
std::string toUpperAscii(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
    return result;
}
}

Board::Board(const map<string, int>& data, int s) : 
    tiles(static_cast<size_t>(s < 0 ? 0 : s)), codeToIndex(data), size(s < 0 ? 0 :s){}

Board::Board(int s) : tiles(static_cast<size_t>(s < 0 ? 0 : s)), size(s < 0 ? 0 : s){
    codeToIndex["initializer"] = -1; //untuk inisialisasi codeToIndex
}

Tile* Board::getTile(int idx){
    if (idx < 0 || idx >= size) {
        return nullptr;
    }

    return tiles[static_cast<size_t>(idx)].get();
};

Tile* Board::getTileByCode(string cd){
    auto mapped = codeToIndex.find(cd);
    if (mapped != codeToIndex.end()) {
        return getTile(mapped->second);
    }

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

void Board::setTileAt(int idx, unique_ptr<Tile> tile) {
    if (idx < 0 || idx >= size) {
        return;
    }
    if (tile == nullptr) {
        return;
    }

    codeToIndex[tile->getCode()] = idx;
    tiles[static_cast<size_t>(idx)] = std::move(tile);
}

int Board::getPlacedTileCount() const {
    int count = 0;
    for (const unique_ptr<Tile>& tile : tiles) {
        if (tile != nullptr) {
            ++count;
        }
    }
    return count;
}

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

void Board::buildFromConfig(const Config& config){
    if (size < 40) {
        size = 40;
    }

    tiles.clear();
    tiles.resize(static_cast<size_t>(size));
    properties.clear();
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
        auto prop = make_unique<StreetProperty>(
            code, name, buyPrice, mortgageValue, color, housePrice, hotelPrice, rentTable);
        StreetProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<StreetTile>(idx, ref));
    };

    auto addRailroad = [this, &config](int idx,
                                       const std::string& code,
                                       const std::string& name,
                                       int buyPrice,
                                       int mortgageValue) {
        auto prop = make_unique<RailroadProperty>(
            code, name, buyPrice, mortgageValue, config.railroad.rentTable);
        RailroadProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<RailroadTile>(idx, ref));
    };

    auto addUtility = [this, &config](int idx,
                                      const std::string& code,
                                      const std::string& name,
                                      int buyPrice,
                                      int mortgageValue) {
        auto prop = make_unique<UtilityProperty>(
            code, name, buyPrice, mortgageValue, config.utility.multiplierTable);
        UtilityProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<UtilityTile>(idx, ref));
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
            setTileAt(idx, make_unique<FestivalTile>(idx, action.code, actionName));
        } else if (action.isTax()) {
            const TaxType taxType = (action.code == "PPH") ? TaxType::PPH : TaxType::PBM;
            setTileAt(idx, make_unique<TaxTile>(idx, action.code, actionName, taxType));
        } else if (action.isChanceCard() || action.isCommunityChest()) {
            const bool isChance = action.isChanceCard();
            setTileAt(idx, make_unique<CardTile>(idx, action.code, actionName, isChance));
        } else if (action.isSpecial()) {
            if (action.code == "GO") {
                setTileAt(idx, make_unique<GoTile>(idx, action.code, actionName));
            } else if (action.code == "PEN") {
                setTileAt(idx, make_unique<JailTile>(idx, action.code, actionName));
            } else if (action.code == "BBP") {
                setTileAt(idx, make_unique<FreeParkingTile>(idx, action.code, actionName));
            } else if (action.code == "PPJ") {
                setTileAt(idx, make_unique<GoToJailTile>(idx, action.code, actionName));
            } else {
                setTileAt(idx, make_unique<ActionTile>(idx, action.code, actionName));
            }
        }
    }
}

void Board::generateDefaultBoard() {
    // Regenerate board from scratch to ensure stable indices and code mapping.
    if (size < 40) {
        size = 40;
    }

    tiles.clear();
    tiles.resize(static_cast<size_t>(size));
    properties.clear();
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
        auto prop = make_unique<StreetProperty>(
            code, name, buyPrice, mortgageValue, color, housePrice, hotelPrice, rentTable);
        StreetProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<StreetTile>(idx, ref));
    };

    auto addRailroad = [this](int idx,
                              const string& code,
                              const string& name,
                              int buyPrice,
                              int mortgageValue,
                              const map<int, int>& rentTable) {
        auto prop = make_unique<RailroadProperty>(code, name, buyPrice, mortgageValue, rentTable);
        RailroadProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<RailroadTile>(idx, ref));
    };

    auto addUtility = [this](int idx,
                             const string& code,
                             const string& name,
                             int buyPrice,
                             int mortgageValue,
                             const map<int, int>& multTable) {
        auto prop = make_unique<UtilityProperty>(code, name, buyPrice, mortgageValue, multTable);
        UtilityProperty& ref = *prop;
        properties.push_back(std::move(prop));
        setTileAt(idx, make_unique<UtilityTile>(idx, ref));
    };

    setTileAt(0, make_unique<GoTile>(0, "GO", "Petak Mulai"));
    addStreet(1, "GRT", "Garut", 60, 30, "Coklat", 50, 50, {2, 10, 30, 90, 160, 250});
    setTileAt(2, make_unique<CardTile>(2, "DNU", "Dana Umum", false));
    addStreet(3, "TSK", "Tasikmalaya", 60, 30, "Coklat", 50, 50, {4, 20, 60, 180, 320, 450});
    setTileAt(4, make_unique<TaxTile>(4, "PPH", "Pajak Penghasilan", TaxType::PPH));
    addRailroad(5, "GBR", "Stasiun Gambir", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(6, "BGR", "Bogor", 100, 50, "Biru_Muda", 50, 50, {6, 30, 90, 270, 400, 550});
    setTileAt(7, make_unique<FestivalTile>(7, "FES", "Festival"));
    addStreet(8, "DPK", "Depok", 100, 50, "Biru_Muda", 50, 50, {6, 30, 90, 270, 400, 550});
    addStreet(9, "BKS", "Bekasi", 120, 60, "Biru_Muda", 50, 50, {8, 40, 100, 300, 450, 600});
    setTileAt(10, make_unique<JailTile>(10, "PEN", "Penjara"));
    addStreet(11, "MGL", "Magelang", 140, 70, "Merah_Muda", 100, 100, {10, 50, 150, 450, 625, 750});
    addUtility(12, "PLN", "PLN", 150, 75, {{1, 4}, {2, 10}});
    addStreet(13, "SOL", "Solo", 140, 70, "Merah_Muda", 100, 100, {10, 50, 150, 450, 625, 750});
    addStreet(14, "YOG", "Yogyakarta", 160, 80, "Merah_Muda", 100, 100, {12, 60, 180, 500, 700, 900});
    addRailroad(15, "STB", "Stasiun Bandung", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(16, "MAL", "Malang", 180, 90, "Orange", 100, 100, {14, 70, 200, 550, 750, 950});
    setTileAt(17, make_unique<CardTile>(17, "DNU", "Dana Umum", false));
    addStreet(18, "SMG", "Semarang", 180, 90, "Orange", 100, 100, {14, 70, 200, 550, 750, 950});
    addStreet(19, "SBY", "Surabaya", 200, 100, "Orange", 100, 100, {16, 80, 220, 600, 800, 1000});
    setTileAt(20, make_unique<FreeParkingTile>(20, "BBP", "Bebas Parkir"));
    addStreet(21, "MKS", "Makassar", 220, 110, "Merah", 150, 150, {18, 90, 250, 700, 875, 1050});
    setTileAt(22, make_unique<CardTile>(22, "KSP", "Kesempatan", true));
    addStreet(23, "BLP", "Balikpapan", 220, 110, "Merah", 150, 150, {18, 90, 250, 700, 875, 1050});
    addStreet(24, "MND", "Manado", 240, 120, "Merah", 150, 150, {20, 100, 300, 750, 925, 1100});
    addRailroad(25, "TUG", "Stasiun Tugu", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    addStreet(26, "PLB", "Palembang", 260, 130, "Kuning", 150, 150, {22, 110, 330, 800, 975, 1150});
    addStreet(27, "PKB", "Pekanbaru", 260, 130, "Kuning", 150, 150, {22, 110, 330, 800, 975, 1150});
    addUtility(28, "PAM", "PAM", 150, 75, {{1, 4}, {2, 10}});
    addStreet(29, "MED", "Medan", 280, 140, "Kuning", 150, 150, {24, 120, 360, 850, 1025, 1200});
    setTileAt(30, make_unique<GoToJailTile>(30, "PPJ", "Pergi ke Penjara"));
    addStreet(31, "BDG", "Bandung", 300, 150, "Hijau", 200, 200, {26, 130, 390, 900, 1100, 1275});
    addStreet(32, "DEN", "Denpasar", 300, 150, "Hijau", 200, 200, {26, 130, 390, 900, 1100, 1275});
    setTileAt(33, make_unique<FestivalTile>(33, "FES", "Festival"));
    addStreet(34, "MTR", "Mataram", 320, 160, "Hijau", 200, 200, {28, 150, 450, 1000, 1200, 1400});
    addRailroad(35, "GUB", "Stasiun Gubeng", 200, 100, {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    setTileAt(36, make_unique<CardTile>(36, "KSP", "Kesempatan", true));
    addStreet(37, "JKT", "Jakarta", 350, 175, "Biru_Tua", 200, 200, {35, 175, 500, 1100, 1300, 1500});
    setTileAt(38, make_unique<TaxTile>(38, "PBM", "Pajak Barang Mewah", TaxType::PBM));
    addStreet(39, "IKN", "Ibu Kota Nusantara", 400, 200, "Biru_Tua", 200, 200, {50, 200, 600, 1400, 1700, 2000});
}



Board::~Board() = default;