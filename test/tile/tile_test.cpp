#include "board/Board.hpp"
#include "tile/PropertyTile.hpp"
#include "property/StreetProperty.hpp"
#include "property/RailroadProperty.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"
#include "core/Dice.hpp"

using namespace std;
#include <vector>

int main(){
    Dice d;
    Board b(20);
    GameEngine ge(20);
    cout << "============= NIMONSPOLI =============\n";

    //setup player
    Player p = Player("tensai", 30);
    Player p2 = Player("sakuragi", 2000);
    vector<Player*> allPlayer;
    allPlayer.push_back(&p);
    allPlayer.push_back(&p2);

    //setup property
    vector<int> x = {100, 150, 200, 250, 300, 350, 400, 450};

    StreetProperty prop("LNDN", "Lundun", 300, 150, "GREEN", 300, 400, x);
    StreetTile st(6, &prop);
    //mungkin bisa simplify konstruktor StreetTile biar pakai property aja? nanti dilihat dependencynya lagi
    StreetProperty prop2("BRM", "Birmingham", 300, 150, "GREEN", 300, 400, x);
    StreetTile st2(7, &prop2);
    
    TurnContext tc(&p, &b, 1, 100, allPlayer); 

    // RailroadProperty rprop2("GMBR", "Gambir", 0, 150, x);


    p2.buy(&prop);
    prop.setOwner(&p2); // ini harus dihandle di buy
    st.onLanded(tc); 
    // TurnContext tc2(&p2, &d, &b, &ge); //gimana nanti modif turncontextnya yah
    tc.nextTurn();
    st2.onLanded(tc);
    return 0;
}