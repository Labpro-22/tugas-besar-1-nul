#include "card.hpp"
class SkillCard : public Card {
protected:
    int remainingDuration; 

public:
    SkillCard(std::string desc, int duration) ;

    int getDuration() const ;

    void decreaseDuration() ;

    void execute(Player* player, TurnContext& ctx) override ;

    virtual void use(Player* player, TurnContext& ctx) = 0;
};