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
    PBM
};

class ActionTile : public Tile{
    public:
        ActionTile(int idx, string cd, string nm);
        virtual void onLanded(TurnContext& ctx);
};

class GoTile : public ActionTile{
    public:
        GoTile(int idx, string cd, string nm);
        void onLanded(TurnContext& ctx) override;
        void paySalary(Player* player, int amount);
};

class JailTile : public ActionTile{
    private:
        vector<Player*> inmates;
        vector<Player*> visitors;
    public:
        JailTile(int idx, string cd, string nm);
        void onLanded(TurnContext& ctx) override;
        void addInmate(Player* player);
        void removeInmate(Player* player);
        bool isInmate(Player* player);
};

class FreeParkingTile : public ActionTile{
    public:
        FreeParkingTile(int idx, string cd, string nm);
        void onLanded(TurnContext& ctx) override;
};

class GoToJailTile : public ActionTile{
    public:
        GoToJailTile(int idx, string cd, string nm);
        void onLanded(TurnContext& ctx) override;
};

class CardTile : public ActionTile{
    private:
        bool isChance; // true = Kesempatan, false = Dana Umum
    public:
        CardTile(int idx, string cd, string nm, bool chance);
        void onLanded(TurnContext& ctx) override;
};

class FestivalTile : public ActionTile{
    public:
        FestivalTile(int idx, string cd, string nm);
        void onLanded(TurnContext& ctx) override;
        void applyFestival(Player* player, StreetProperty* prop);
};

class TaxTile : public ActionTile{
    private:
        TaxType taxType;
    public: 
        TaxTile(int idx, string cd, string nm, TaxType type);
        void onLanded(TurnContext& ctx) override;
        void applyPPH(Player& player);
        void applyPPN(Player& player);
        void applyPBM(Player& player);
};
