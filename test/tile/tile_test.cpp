#include "board/Board.hpp"
#include "tile/ActionTile.hpp"
#include "tile/PropertyTile.hpp"
#include "property/StreetProperty.hpp"
#include "property/RailroadProperty.hpp"
#include "property/UtilityProperty.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "core/TurnManager.hpp"
#include "core/Dice.hpp"
#include "player/Player.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>

int main() {
    Board board(20);

    board.setTileAt(0, std::make_unique<GoTile>(0, "GO", "Petak Mulai"));
    board.setTileAt(1, std::make_unique<ActionTile>(1, "A1", "Action 1"));
    board.setTileAt(2, std::make_unique<CardTile>(2, "C1", "Kesempatan"));

    std::vector<int> rentTable{10, 20, 30, 40, 50, 60};
    auto streetA = std::make_unique<StreetProperty>(
        "S1", "Street One", 100, 50, "Hijau", 50, 100, rentTable);
    auto streetB = std::make_unique<StreetProperty>(
        "S2", "Street Two", 120, 60, "Hijau", 50, 100, rentTable);

    std::map<int, int> rrRent{{1, 25}, {2, 50}, {3, 100}, {4, 200}};
    auto railroad = std::make_unique<RailroadProperty>(
        "R1", "Railroad One", 200, 100, rrRent);

    std::map<int, int> utilMult{{1, 4}, {2, 10}};
    auto utility = std::make_unique<UtilityProperty>(
        "U1", "Utility One", 150, 75, utilMult);

    board.setTileAt(3, std::make_unique<StreetTile>(3, *streetA));
    board.setTileAt(4, std::make_unique<StreetTile>(4, *streetB));
    board.setTileAt(5, std::make_unique<RailroadTile>(5, *railroad));
    board.setTileAt(6, std::make_unique<UtilityTile>(6, *utility));

    for (int i = 7; i < board.getSize(); ++i) {
        board.setTileAt(i, std::make_unique<ActionTile>(
            i, "A" + std::to_string(i), "Action " + std::to_string(i)));
    }

    assert(board.getPlacedTileCount() == board.getSize());

    for (int i = 0; i < board.getSize(); ++i) {
        Tile* tile = board.getTile(i);
        assert(tile != nullptr);
        assert(tile->getIndex() == i);
    }

    assert(board.getTileByCode("GO") != nullptr);
    assert(board.getTileByCode("S1") != nullptr);
    assert(board.getTileByCode("S2") != nullptr);
    assert(board.getTileByCode("R1") != nullptr);
    assert(board.getTileByCode("U1") != nullptr);
    assert(board.getTileByCode("DOES_NOT_EXIST") == nullptr);

    const std::vector<StreetTile*> hijauGroup = board.getColorGroup("HIJAU");
    assert(hijauGroup.size() == 2);

    std::cout << "[PASS] Board tile storage test passed.\n";

    //===================================================== TILE =================================================
    Dice d;
    Board b(20);
    GameEngine ge(20);
    std::cout << "============= NIMONSPOLI =============\n";

    //setup player
    Player p("tensai", 30);
    Player p2("sakuragi", 2000);
    std::vector<Player*> allPlayer;
    allPlayer.push_back(&p);
    allPlayer.push_back(&p2);

    //setup property
    std::vector<int> x = {100, 150, 200, 250, 300, 350, 400, 450};

    StreetProperty prop("LNDN", "Lundun", 300, 150, "GREEN", 300, 400, x);
    StreetTile st(6, prop);
    StreetProperty prop2("BRM", "Birmingham", 300, 150, "GREEN", 300, 400, x);
    StreetTile st2(7, prop2);
    
    TurnContext tc(p, d, b, ge); 
    
    // PERBAIKAN 1: Gunakan reference (&) agar tidak meng-copy TurnManager!
    TurnManager& tm = tc.getTurnMgr();

    // PERBAIKAN 2: Masukkan urutan player ke TurnManager agar nextTurn bisa berjalan!
    tm.setTurnOrder(allPlayer);

    std::map<int, int> x_map = {{1, 40}, {2, 100}, {3, 200}};

    RailroadProperty rprop("GMBR", "Gambir", 0, 150, x_map);
    RailroadProperty rprop2("GBNG", "Gubeng", 0, 200, x_map);
    RailroadTile rt(8, rprop);
    RailroadTile rt2(9, rprop2);

    UtilityProperty uprop("WTRC", "Water Company", 0, 150, x_map);
    UtilityProperty uprop2("PLN", "Electricity Company", 0, 200, x_map);
    UtilityTile ut(10, uprop);
    UtilityTile ut2(11, uprop2);

    p2.buy(&prop);
    prop.setOwner(&p2); // ini harus dihandle di buy
    
    std::cout << "\n--- Turn 1 ---\n";
    st.onLanded(tc); 
    
    std::cout << "\n--- Turn 2 ---\n";
    tm.nextTurn(tc);
    st2.onLanded(tc);
    
    std::cout << "\n--- Turn 3 ---\n";
    tm.nextTurn(tc);
    rt.onLanded(tc);
    
    std::cout << "\n--- Turn 4 ---\n";
    tm.nextTurn(tc);
    rt2.onLanded(tc);

    std::cout << "\n--- Turn 5 ---\n";
    tm.nextTurn(tc);
    ut.onLanded(tc);
    
    std::cout << "\n--- Turn 6 ---\n";
    tm.nextTurn(tc);
    ut.onLanded(tc);
    
    return 0;
}