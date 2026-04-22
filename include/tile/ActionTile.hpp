#pragma once

#include "Tile.hpp"
#include <iostream>
#include <vector>


class Player;
class TurnContext;
class StreetProperty;

// =================================== dummy classes =====================================
class CardType{
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