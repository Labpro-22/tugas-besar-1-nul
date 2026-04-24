#pragma once

#include "SkillCard.hpp"

class TurnContext;

class DemolitionCard : public SkillCard {
  public:
    DemolitionCard();

    void apply(TurnContext& ctx) override;
};
