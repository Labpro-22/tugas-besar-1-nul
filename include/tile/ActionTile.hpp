#ifndef ACTION_TILE_HPP
#define ACTION_TILE_HPP

#pragma once

#include "../board/Board.hpp" //nanti dihapus kalau ga dibutuhkan
#include "Tile.hpp"
#include <iostream>
#include <vector>


// =================================== dummy classes =====================================
class Player{
    //dummy data
};

class TurnContext{
    //dummy data
};

class CardType{
    //dummy data
};

class StreetProperty{
    //dummy data
};

enum class TaxType{
    PPH,
    PPN
};

// ======================================================================================

class ActionTile : public Tile{
    public:
        virtual void onLanded(Player* player, TurnContext& ctx);
};

class GoTile : public ActionTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void paySalary(Player* player);
};

class JailTile : public ActionTile{
    private:
        vector<Player*> inmates;
        vector<Player*> visitors;
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void addInmate(Player* player);
        void removeInmate(Player* player);
        bool isInmate(Player* player);
};

class FreeParkingTile : public ActionTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
};

class GoToJailTile : public ActionTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
};

class CardTile : public ActionTile{
    private:
        CardType type;
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
};

class FestivalTile : public ActionTile{
    public:
        void onLanded(Player* player, TurnContext& ctx) override;
        void applyFestival(Player* player, StreetProperty* prop);
};

class TaxTile : public ActionTile{
    private:
        TaxType taxType;
    public: 
        void onLanded(Player* player, TurnContext& ctx) override;
        void applyPPH(Player* player);
        void applyPPN(Player* player);
};


#endif