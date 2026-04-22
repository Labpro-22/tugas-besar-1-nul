#include "card/ChanceCard.hpp"

ChanceCard::ChanceCard(std::string desc, ChanceType t) : Card(std::move(desc)), type(t) {}

void ChanceCard::execute(Player* player, TurnContext& ctx) {
    std::cout << "[Kartu Kesempatan] " << description << "\n";
    switch (type){
        case ChanceType::GO_TO_NEAREST_STATION:
            std::cout << "Efek: Pergi ke stasiun terdekat.\n";
            //lanjut
            break;
        case ChanceType::MOVE_BACK_3:
            std::cout << "Efek: Mundur 3 petak.\n";
            //lanjut
            break;
        case ChanceType::GO_TO_JAIL:
            std::cout << "Efek: Masuk Penjara.\n";
            //lanjut
            break;
    }
}
