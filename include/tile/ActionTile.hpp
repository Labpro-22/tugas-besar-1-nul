#pragma once

#include "Tile.hpp"
#include "card/card.hpp"
#include <iostream>
#include <vector>

class Player;
class TurnContext;
class StreetProperty;
class Property;

enum class TaxType { PPH, PBM };

class ActionTile : public Tile {
  public:
    ActionTile(int idx, string cd, string nm);
    virtual void onLanded(TurnContext& ctx);
};

class GoTile : public ActionTile {
  public:
    GoTile(int idx, string cd, string nm);
};

class JailTile : public ActionTile {
  private:
    vector<Player*> inmates;
    vector<Player*> visitors;

  public:
    JailTile(int idx, string cd, string nm);
    void onLanded(TurnContext& ctx) override;
    void addInmate(Player& player);
    void removeInmate(Player& player);
    bool isInmate(Player& player);
};

class FreeParkingTile : public ActionTile {
  public:
    FreeParkingTile(int idx, string cd, string nm);
};

class GoToJailTile : public ActionTile {
  public:
    GoToJailTile(int idx, string cd, string nm);
    void onLanded(TurnContext& ctx) override;
};

enum class CardTileType {
    CHANCE,         // Kartu Kesempatan
    COMMUNITY_CHEST // Kartu Dana Umum
};

class CardTile : public ActionTile{
    private:
        CardTileType cardType;
    public:
        CardTile(int idx, string cd, string nm);
        CardTile(int idx, string cd, string nm, CardTileType type);
        void onLanded(TurnContext& ctx) override;
        void setCardType(CardTileType type);
        CardTileType getCardType() const;
};

class FestivalTile : public ActionTile {
  public:
    FestivalTile(int idx, string cd, string nm);
    void onLanded(TurnContext& ctx) override;
    void applyFestival(Player& player, Property& prop, TurnContext& ctx);
};

class TaxTile : public ActionTile {
  private:
    TaxType taxType;

  public:
    TaxTile(int idx, string cd, string nm, TaxType type);
    void onLanded(TurnContext& ctx) override;
    void applyPPH(Player& player, TurnContext& ctx);
    void applyPBM(Player& player, TurnContext& ctx);
};
