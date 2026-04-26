#pragma once

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Forward declarations
class Board;
class Player;
class TurnContext;

class Tile {
  protected:
    int index;
    string code;
    string name;

  public:
    Tile(int idx, string cd, string nm);
    int getIndex() const;
    string getCode() const;
    string getName() const;
    string getDisplay() const;
    virtual void onLanded(TurnContext& ctx) = 0;
};
