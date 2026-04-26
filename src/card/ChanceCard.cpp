#include "card/ChanceCard.hpp"
#include "player/Player.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"
#include "tile/PropertyTile.hpp"
#include "property/RailroadProperty.hpp"
#include "exception/InvalidGameStateException.hpp"

#include <iostream>
#include <limits>

ChanceCard::ChanceCard(std::string desc, ChanceType t)
    : Card(std::move(desc)), type(t) {}

void ChanceCard::execute(Player* player, TurnContext& ctx) {
    std::cout << "[Kartu Kesempatan] " << description << "\n";
    
    if (player == nullptr) {
        throw InvalidGameStateException("Cannot execute ChanceCard: player is null");
    }
    
    switch (type) {
        case ChanceType::GO_TO_NEAREST_STATION: {
            // std::cout << "Efek: Pergi ke stasiun terdekat.\n";
            
            int currentPos = player->getPosition();
            int boardSize = ctx.getBoardSize();
            int nearestStationPos = -1;
            int minDistance = boardSize;
            
            Board& board = ctx.board;
            for (int i = 1; i <= boardSize; i++) {
                int checkPos = (currentPos + i) % boardSize;
                Tile* tile = board.getTile(checkPos);
                
                if (tile != nullptr) {
                    RailroadTile* railTile = dynamic_cast<RailroadTile*>(tile);
                    if (railTile != nullptr) {
                        nearestStationPos = checkPos;
                        break; 
                    }
                }
            }
            
            if (nearestStationPos != -1) {
                std::cout << "[INFO] Stasiun terdekat ditemukan di posisi " << nearestStationPos << "\n";
                player->moveForwardTo(nearestStationPos, ctx);
            } else {
                std::cout << "[INFO] Tidak ada stasiun di papan permainan.\n";
            }
            break;
        }
        
        case ChanceType::MOVE_BACK_3: {
            // std::cout << "Efek: Mundur 3 petak.\n";
            
            int currentPos = player->getPosition();
            int boardSize = ctx.getBoardSize();
            int newPos = (currentPos - 3 + boardSize) % boardSize;
            
            std::cout << "[INFO] Pemain mundur dari posisi " << currentPos << " ke posisi " << newPos << "\n";
            player->moveBackwardTo(newPos, ctx);
            break;
        }
        
        case ChanceType::GO_TO_JAIL: {
            // std::cout << "Efek: Masuk Penjara.\n";
            
            Board& board = ctx.board;
            int boardSize = ctx.getBoardSize();
            int jailPos = -1;
            
            for (int i = 0; i < boardSize; i++) {
                Tile* tile = board.getTile(i);
                if (tile != nullptr && tile->getCode() == "PEN") {
                    jailPos = i;
                    break;
                }
            }
            
            if (jailPos != -1) {
                std::cout << "[INFO] Pemain dipindahkan ke Penjara.\n";
                player->moveForwardTo(jailPos, ctx);
                player->enterJail();
            } else {
                std::cout << "[INFO] Pemain dipindahkan ke Penjara.\n";
                player->moveForwardTo(10, ctx);
                player->enterJail();
            }
            break;
        }
    }
}
