#include "SkillCard.hpp"

SkillCard::SkillCard(std::string desc, int duration) : Card(std::move(desc)), remainingDuration(duration) {}

int SkillCard::getDuration() const {
    return remainingDuration      ;
}

void SkillCard::decreaseDuration() {
    if (remainingDuration > 0) remainingDuration--;
}

void SkillCard::execute(Player* player, TurnContext& ctx)  {
    std::cout << "[MENDAPATKAN KEMAMPUAN] " << description << " ditambahkan ke tangan.\n";
        
    // TODO: Tambahkan kartu ini ke vector<SkillCard*> hand milik player
    // player->addSkillCard(this);
    // TODO: Jika hand > 3 setelah ditambah, trigger wajib buang (drop) 1 kartu
}
