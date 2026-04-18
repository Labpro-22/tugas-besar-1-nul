#include "board/Board.hpp"
#include "tile/PropertyTile.hpp"
#include "property/StreetProperty.hpp"
#include "core/TurnContext.hpp"
#include "core/GameEngine.hpp"

using namespace std;
#include <vector>

int main(){
    Dice d;
    Board b(20);
    GameEngine ge;
    cout << "Hello, World\n";

    Player p = Player("tensai", 30);
    Player p2 = Player("sakuragi", 2000);

    vector<int> x = {100, 150, 200, 250, 300, 350, 400, 450};

    StreetProperty prop("LNDN", "Lundun", 300, 150, "GREEN", 300, 400, x);
    StreetTile st(6, "LNDN", "Lundun", "GREEN", &prop);
    //mungkin bisa simplify konstruktor StreetTile biar pakai property aja? nanti dilihat dependencynya lagi
    StreetProperty prop2("BRM", "Birmingham", 300, 150, "GREEN", 300, 400, x);
    StreetTile st2(7, "BRM", "Birmingham", "GREEN", &prop2);

    TurnContext tc(&p, &d, &b, &ge); 

    p2.buy(&prop);
    prop.setOwner(&p2); // ini harus dihandle di buy
    st.onLanded(&p, tc); 
    TurnContext tc2(&p2, &d, &b, &ge); //gimana nanti modif turncontextnya yah
    st2.onLanded(&p2, tc2);
    return 0;
}