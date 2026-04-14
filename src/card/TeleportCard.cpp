#include "TeleportCard.hpp"

TeleportCard::TeleportCard() : SkillCard("TeleportCard: Pindah ke petak manapun", 0) {}

void TeleportCard::use(Player* player, TurnContext& ctx)  {
    std::cout << "[MENGGUNAKAN] TeleportCard: Memulai proses teleportasi.\n";
    // TODO: Tampilkan daftar petak / minta input kode petak dari user
    // TODO: Pindahkan player langsung ke petak tujuan tanpa melewati petak lain (tidak dapat gaji GO)
    // TODO: Eksekusi onLanded pada petak tujuan
}