#include "gui/GuiPanelManager.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <sstream>

#include "board/Board.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "core/TransactionLogger.hpp"
#include "player/Player.hpp"
#include "player/BotPlayer.hpp"
#include "property/Property.hpp"
#include "property/StreetProperty.hpp"
#include "property/RailroadProperty.hpp"
#include "property/UtilityProperty.hpp"
#include "property/PropertyStatus.hpp"
#include "tile/PropertyTile.hpp"
#include "core/AuctionManager.hpp"
#include "core/BankruptcyManager.hpp"
#include "card/ChanceCard.hpp"
#include "card/CommunityChestCard.hpp"

#if NIMONSPOLI_HAS_RAYLIB
#include <raylib.h>
#endif

namespace GuiPanels {

namespace {
constexpr int kFontBody = 16;
constexpr int kFontSmall = 14;
constexpr int kFontTitle = 24;

Color panelBgColor() { return Color{35, 35, 48, 255}; }
Color panelBorderColor() { return Color{100, 100, 130, 255}; }
Color panelHeaderColor() { return Color{45, 45, 62, 255}; }

// Strip ANSI escape codes for GUI display
std::string stripAnsi(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\033') {
            while (i < input.size() && !std::isalpha(input[i])) ++i;
            continue;
        }
        out += input[i];
    }
    return out;
}
} // namespace

// ============================================================================
// TextInput
// ============================================================================

void PanelManager::TextInput::update() {
#if NIMONSPOLI_HAS_RAYLIB
    if (!active) return;
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) text.push_back(static_cast<char>(key));
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!text.empty()) text.pop_back();
        backspaceTimer = 0.0f;
    } else if (IsKeyDown(KEY_BACKSPACE)) {
        backspaceTimer += GetFrameTime();
        if (backspaceTimer > 0.4f) {
            static float rep = 0.0f;
            rep += GetFrameTime();
            if (rep > 0.05f) {
                rep = 0.0f;
                if (!text.empty()) text.pop_back();
            }
        }
    } else {
        backspaceTimer = 0.0f;
    }
#endif
}

void PanelManager::TextInput::draw(float x, float y, float w, float h, const char* hint) {
#if NIMONSPOLI_HAS_RAYLIB
    Color bg = active ? Color{240, 248, 255, 255} : WHITE;
    DrawRectangleRec(Rectangle{x, y, w, h}, bg);
    DrawRectangleLinesEx(Rectangle{x, y, w, h}, 2.0f, active ? DARKBLUE : GRAY);
    const char* disp = text.empty() && hint ? hint : text.c_str();
    Color tc = text.empty() && hint ? GRAY : BLACK;
    DrawText(disp, static_cast<int>(x + 10), static_cast<int>(y + h / 2 - 10), 20, tc);
    if (active && static_cast<int>(GetTime() * 2.5f) % 2 == 0) {
        int tw = MeasureText(text.c_str(), 20);
        DrawLine(static_cast<int>(x + 10 + tw), static_cast<int>(y + 6),
                 static_cast<int>(x + 10 + tw), static_cast<int>(y + h - 6), BLACK);
    }
#endif
}

bool PanelManager::TextInput::checkClick(float x, float y, float w, float h) {
#if NIMONSPOLI_HAS_RAYLIB
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        bool inside = CheckCollisionPointRec(m, Rectangle{x, y, w, h});
        active = inside;
        return inside;
    }
#endif
    return false;
}

// ============================================================================
// PanelManager core
// ============================================================================

PanelManager::PanelManager() = default;

bool PanelManager::isActive() const { return active_ != PanelType::NONE; }
bool PanelManager::isBlocking() const { return blocking_; }
PanelType PanelManager::getType() const { return active_; }

void PanelManager::dismiss() {
    active_ = PanelType::NONE;
    blocking_ = false;
}

// ============================================================================
// Panel requests
// ============================================================================

void PanelManager::showPropertyBuy(Player& player, Property& prop, TurnContext& ctx) {
    active_ = PanelType::PROPERTY_BUY;
    blocking_ = true;
    buy_ = PropertyBuyState{&player, &prop, &ctx, ActionResult::PENDING};
}

void PanelManager::showAuction(Property& prop, const std::vector<Player*>& players,
                               int startIdx, TurnContext& ctx) {
    active_ = PanelType::AUCTION;
    blocking_ = true;
    auction_ = AuctionState{&prop, {}, startIdx, -1, nullptr, 0, 0, 0, "", ActionResult::PENDING, "", false, 0.0f, &ctx};
    auction_.bidders.clear();
    int n = static_cast<int>(players.size());
    int begin = ((startIdx % n) + n) % n;
    for (int k = 0; k < n; ++k) {
        int idx = (begin + k) % n;
        Player* p = players[idx];
        if (p && p->getStatus() != PlayerStatus::BANKRUPT) {
            auction_.bidders.push_back(p);
        }
    }
    auction_.activeCount = static_cast<int>(auction_.bidders.size());
    auction_.statusMsg = "Lelang dimulai untuk " + prop.getName();
}

void PanelManager::showBankruptcy(TurnContext& ctx, int debtAmount) {
    active_ = PanelType::BANKRUPTCY;
    blocking_ = true;
    bankruptcy_ = BankruptcyState{&ctx, debtAmount, ActionResult::PENDING, {}};
    // Options will be populated on first render
}

void PanelManager::showUpgrade(Player& player, TurnContext& ctx) {
    active_ = PanelType::UPGRADE;
    blocking_ = true;
    upgrade_ = UpgradeState{&player, &ctx, {}, ActionResult::PENDING};
    for (Property* prop : player.getProperties()) {
        if (StreetProperty* sp = dynamic_cast<StreetProperty*>(prop)) {
            if (sp->canBuild()) upgrade_.eligible.push_back(prop);
        }
    }
}

void PanelManager::showSkillCard(Player& player, TurnContext& ctx) {
    active_ = PanelType::SKILL_CARD;
    blocking_ = true;
    skill_ = SkillCardState{&player, &ctx, ActionResult::PENDING, -1};
}

void PanelManager::showMortgage(Player& player) {
    active_ = PanelType::MORTGAGE;
    blocking_ = true;
    mortgage_ = MortgageState{&player, ActionResult::PENDING, {}};
    for (Property* prop : player.getProperties()) {
        if (prop && prop->getStatus() == PropertyStatus::OWNED) {
            mortgage_.eligible.push_back(prop);
        }
    }
}

void PanelManager::showDismortgage(Player& player) {
    active_ = PanelType::DISMORTGAGE;
    blocking_ = true;
    dismortgage_ = DismortgageState{&player, ActionResult::PENDING, {}};
    for (Property* prop : player.getProperties()) {
        if (prop && prop->getStatus() == PropertyStatus::MORTGAGED) {
            dismortgage_.eligible.push_back(prop);
        }
    }
}

void PanelManager::showSave(GameEngine& engine) {
    active_ = PanelType::SAVE;
    blocking_ = true;
    save_ = SaveState{&engine, ActionResult::PENDING, "", false};
    textInput_.text.clear();
    textInput_.active = true;
}

void PanelManager::showPropertyCert(Board& board) {
    active_ = PanelType::PROPERTY_CERT;
    blocking_ = true;
    cert_ = PropertyCertState{&board, ActionResult::PENDING, "", false};
    textInput_.text.clear();
    textInput_.active = true;
}

void PanelManager::showProfile(Player& player) {
    active_ = PanelType::PROFILE;
    blocking_ = false;
    profile_ = ProfileState{&player};
}

void PanelManager::showPropertiesList(Player& player) {
    active_ = PanelType::PROPERTIES_LIST;
    blocking_ = false;
    properties_ = PropertiesListState{&player};
}

void PanelManager::showLog(TransactionLogger& logger) {
    active_ = PanelType::LOG;
    blocking_ = false;
    log_ = LogState{&logger};
}

void PanelManager::showMessage(const std::string& title, const std::string& message) {
    active_ = PanelType::MESSAGE;
    blocking_ = true;
    message_ = MessageState{title, message, false};
}

void PanelManager::showFestival(Player& player, TurnContext& ctx) {
    active_ = PanelType::FESTIVAL;
    blocking_ = true;
    festival_ = FestivalState{&player, &ctx, {}, ActionResult::PENDING};
    for (Property* prop : player.getProperties()) {
        if (prop && prop->getStatus() == PropertyStatus::OWNED) {
            festival_.ownedProps.push_back(prop);
        }
    }
}

void PanelManager::showTax(Player& player, int taxFlat, float taxPercent) {
    active_ = PanelType::TAX;
    blocking_ = true;
    tax_ = TaxState{&player, taxFlat, taxPercent, ActionResult::PENDING};
}

void PanelManager::showCardReveal(ChanceCard* chanceCard, CommunityChestCard* communityCard, 
                                   Player& player, TurnContext& ctx) {
    active_ = PanelType::CARD_REVEAL;
    blocking_ = true;
    
    std::string title, description, cardType;
    if (chanceCard) {
        title = "Kartu Kesempatan";
        description = chanceCard->getDescription();
        cardType = "chance";
    } else {
        title = "Kartu Dana Umum";
        description = communityCard->getDescription();
        cardType = "community";
    }
    
    cardReveal_ = CardRevealState{title, description, cardType, ActionResult::PENDING, false,
                                  chanceCard, communityCard, &player, &ctx};
}

// ============================================================================
// updateAndRender dispatcher
// ============================================================================

void PanelManager::updateAndRender() {
#if NIMONSPOLI_HAS_RAYLIB
    switch (active_) {
    case PanelType::PROPERTY_BUY: renderPropertyBuy(); break;
    case PanelType::AUCTION: renderAuction(); break;
    case PanelType::BANKRUPTCY: renderBankruptcy(); break;
    case PanelType::UPGRADE: renderUpgrade(); break;
    case PanelType::SKILL_CARD: renderSkillCard(); break;
    case PanelType::MORTGAGE: renderMortgage(); break;
    case PanelType::DISMORTGAGE: renderDismortgage(); break;
    case PanelType::SAVE: renderSave(); break;
    case PanelType::PROPERTY_CERT: renderPropertyCert(); break;
    case PanelType::PROFILE: renderProfile(); break;
    case PanelType::PROPERTIES_LIST: renderPropertiesList(); break;
    case PanelType::LOG: renderLog(); break;
    case PanelType::MESSAGE: renderMessage(); break;
    case PanelType::FESTIVAL: renderFestival(); break;
    case PanelType::TAX: renderTax(); break;
    case PanelType::CARD_REVEAL: renderCardReveal(); break;
    default: break;
    }
#endif
}

// ============================================================================
// Static helpers
// ============================================================================

bool PanelManager::button(const char* text, float x, float y, float w, float h,
                          Color bg, Color textColor, Color hover) {
#if NIMONSPOLI_HAS_RAYLIB
    Rectangle b = {x, y, w, h};
    Vector2 m = GetMousePosition();
    bool over = CheckCollisionPointRec(m, b);
    bool clicked = over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Color cur = over ? hover : bg;
    DrawRectangleRec(b, cur);
    DrawRectangleLinesEx(b, 2.0f, Color{60, 60, 60, 255});
    int fs = static_cast<int>(h * 0.4f);
    int tw = MeasureText(text, fs);
    float tx = x + (w - tw) * 0.5f;
    float ty = y + (h - fs) * 0.5f;
    DrawText(text, static_cast<int>(tx), static_cast<int>(ty), fs, textColor);
    return clicked;
#else
    (void)text; (void)x; (void)y; (void)w; (void)h; (void)bg; (void)textColor; (void)hover;
    return false;
#endif
}

void PanelManager::centeredText(const char* text, int y, int fontSize, Color color) {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int tw = MeasureText(text, fontSize);
    DrawText(text, (sw - tw) / 2, y, fontSize, color);
#endif
}

void PanelManager::wrappedText(const std::string& text, int x, int y, int maxWidth,
                               int fontSize, Color color, int lineHeight) {
#if NIMONSPOLI_HAS_RAYLIB
    std::string line, word;
    int cy = y;
    for (size_t i = 0; i <= text.size(); ++i) {
        char ch = (i < text.size()) ? text[i] : ' ';
        if (ch == ' ' || ch == '\n' || i == text.size()) {
            if (!word.empty()) {
                int lw = MeasureText((line + " " + word).c_str(), fontSize);
                if (!line.empty() && lw > maxWidth) {
                    DrawText(line.c_str(), x, cy, fontSize, color);
                    cy += lineHeight;
                    line = word;
                } else {
                    if (!line.empty()) line += " ";
                    line += word;
                }
                word.clear();
            }
            if (ch == '\n' && !line.empty()) {
                DrawText(line.c_str(), x, cy, fontSize, color);
                cy += lineHeight;
                line.clear();
            }
        } else {
            word += ch;
        }
    }
    if (!line.empty()) DrawText(line.c_str(), x, cy, fontSize, color);
#endif
}

void PanelManager::panelBg(int w, int h, const char* title) {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float x = (sw - w) / 2.0f;
    float y = (sh - h) / 2.0f;
    DrawRectangle(0, 0, sw, sh, Color{0, 0, 0, 180});
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), w, h, panelBgColor());
    DrawRectangleLinesEx(Rectangle{x, y, static_cast<float>(w), static_cast<float>(h)}, 2.0f, panelBorderColor());
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), w, 40, panelHeaderColor());
    int tw = MeasureText(title, kFontTitle);
    DrawText(title, static_cast<int>(x + (w - tw) / 2), static_cast<int>(y + 8), kFontTitle, WHITE);
#endif
}

bool PanelManager::isMouseOver(float x, float y, float w, float h) {
#if NIMONSPOLI_HAS_RAYLIB
    Vector2 m = GetMousePosition();
    return CheckCollisionPointRec(m, Rectangle{x, y, w, h});
#else
    return false;
#endif
}

// ============================================================================
// PROPERTY BUY
// ============================================================================

void PanelManager::renderPropertyBuy() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 300;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Beli Properti");

    if (!buy_.property || !buy_.player) { dismiss(); return; }

    Property* p = buy_.property;
    std::string info = p->getName() + " (" + p->getCode() + ")\n"
                       "Harga: M" + std::to_string(p->getBuyPrice()) + "\n"
                       "Saldo Anda: M" + std::to_string(buy_.player->getBalance());
    wrappedText(info, static_cast<int>(px + 20), static_cast<int>(py + 60), pw - 40, kFontBody, LIGHTGRAY, 24);

    bool canBuy = buy_.player->getBalance() >= p->getBuyPrice();
    float btnY = py + ph - 70;
    if (canBuy && button("Beli (Y)", px + 30, btnY, 140, 45,
                         Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
        try {
            buy_.player->buy(p, *buy_.ctx);
        } catch (const std::exception& e) {
            showMessage("Gagal Membeli", e.what());
            buy_.result = ActionResult::CANCELLED;
            return;
        }
        buy_.result = ActionResult::DONE;
        dismiss();
    }
    if (button("Lewati (N)", px + pw - 170, btnY, 140, 45,
               Color{244, 67, 54, 255}, WHITE, Color{255, 120, 100, 255})) {
        buy_.result = ActionResult::CANCELLED;
        dismiss();
        // Trigger GUI auction
        TurnManager tm = buy_.ctx->getTurnMgr();
        showAuction(*p, tm.getTurnOrder(), tm.getActivePlayerIndex(), *buy_.ctx);
    }
#endif
}

// ============================================================================
// AUCTION
// ============================================================================

void PanelManager::renderAuction() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 550, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Lelang Properti");

    if (!auction_.property || auction_.bidders.empty()) { dismiss(); return; }

    Property* prop = auction_.property;
    std::string info = prop->getName() + " (" + prop->getCode() + ")\n"
                       "Bid Tertinggi: " + (auction_.highestBid < 0 ? "-" : "M" + std::to_string(auction_.highestBid)) + "\n"
                       "Pemenang Sementara: " + (auction_.winner ? auction_.winner->getUsername() : "-") + "\n"
                       "Giliran: " + auction_.bidders[auction_.turnIdx]->getUsername();
    wrappedText(info, static_cast<int>(px + 20), static_cast<int>(py + 60), pw - 40, kFontBody, LIGHTGRAY, 24);

    Player* current = auction_.bidders[auction_.turnIdx];

    // Bot turn
    if (current->isBot()) {
        auction_.botDelay += GetFrameTime();
        if (auction_.botDelay > 0.8f) {
            auction_.botDelay = 0.0f;
            BotPlayer* bot = dynamic_cast<BotPlayer*>(current);
            int bid = bot->decideBid(auction_.highestBid, prop->getBuyPrice(), bot->getBalance());
            if (bid < 0) {
                ++auction_.consecutivePasses;
                if (auction_.winner && auction_.consecutivePasses >= auction_.activeCount - 1) {
                    try {
                        auction_.winner->buy(auction_.property, auction_.highestBid, *auction_.ctx);
                    } catch (const std::exception& e) {
                        showMessage("Lelang Gagal", e.what());
                    }
                    auction_.result = ActionResult::DONE;
                    dismiss();
                    return;
                }
            } else {
                auction_.highestBid = bid;
                auction_.winner = current;
                auction_.consecutivePasses = 0;
            }
            auction_.turnIdx = (auction_.turnIdx + 1) % auction_.activeCount;
        }
    } else {
        // Human turn
        float btnY = py + ph - 130;
        textInput_.update();
        textInput_.draw(px + 30, btnY, 200, 40, "Bid (angka)");
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            textInput_.checkClick(px + 30, btnY, 200, 40);
        }

        if (button("Bid", px + 250, btnY, 100, 40,
                   Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
            try {
                int val = std::stoi(textInput_.text);
                if (val >= 0 && (auction_.highestBid < 0 || val > auction_.highestBid) && val <= current->getBalance()) {
                    auction_.highestBid = val;
                    auction_.winner = current;
                    auction_.consecutivePasses = 0;
                    textInput_.text.clear();
                    auction_.turnIdx = (auction_.turnIdx + 1) % auction_.activeCount;
                }
            } catch (...) {}
        }
        if (button("Pass", px + 370, btnY, 100, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            ++auction_.consecutivePasses;
            if (auction_.winner && auction_.consecutivePasses >= auction_.activeCount - 1) {
                try {
                    auction_.winner->buy(auction_.property, auction_.highestBid, *auction_.ctx);
                } catch (const std::exception& e) {
                    showMessage("Lelang Gagal", e.what());
                }
                auction_.result = ActionResult::DONE;
                dismiss();
                return;
            }
            auction_.turnIdx = (auction_.turnIdx + 1) % auction_.activeCount;
        }
    }
#endif
}

// ============================================================================
// BANKRUPTCY
// ============================================================================

void PanelManager::renderBankruptcy() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 550, ph = 500;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Panel Likuidasi");

    if (!bankruptcy_.ctx) { dismiss(); return; }

    Player& player = bankruptcy_.ctx->currentPlayer;

    // Refresh options every frame
    auto options = BankruptcyManager::getAvailableLiquidationOptions(*bankruptcy_.ctx);
    bankruptcy_.options.clear();
    for (const auto& opt : options) {
        if (!opt.property || opt.property->getStatus() != PropertyStatus::OWNED) continue;
        BankruptcyState::Option o;
        o.prop = opt.property;
        o.type = (opt.type == LiquidationOption::SELL) ? 0 : 1;
        o.value = opt.cashValue;
        std::string typeLabel = "LAIN";
        if (StreetProperty* s = dynamic_cast<StreetProperty*>(opt.property)) {
            typeLabel = s->getColorGroup();
        } else if (dynamic_cast<RailroadProperty*>(opt.property)) {
            typeLabel = "STATION";
        } else if (dynamic_cast<UtilityProperty*>(opt.property)) {
            typeLabel = "UTILITY";
        }
        o.label = opt.property->getName() + " (" + opt.property->getCode() + ") [" + typeLabel + "] " +
                  (o.type == 0 ? "Jual M" : "Gadai M") + std::to_string(o.value);
        bankruptcy_.options.push_back(o);
    }

    std::string header = "Uang: M" + std::to_string(player.getBalance()) +
                         " | Kewajiban: M" + std::to_string(bankruptcy_.debtAmount);
    DrawText(header.c_str(), static_cast<int>(px + 20), static_cast<int>(py + 55), kFontBody, YELLOW);

    float listY = py + 90;
    float itemH = 32;
    for (size_t i = 0; i < bankruptcy_.options.size(); ++i) {
        const auto& o = bankruptcy_.options[i];
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        DrawText(o.label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 6), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            try {
                if (o.type == 0) {
                    int saleValue = o.prop->sellToBank();
                    player.addCash(saleValue);
                    player.sell(*o.prop);
                } else {
                    player.mortgage(o.prop);
                }
            } catch (const std::exception& e) {
                showMessage("Likuidasi Gagal", e.what());
            }
        }
    }

    if (player.getBalance() >= bankruptcy_.debtAmount) {
        bankruptcy_.result = ActionResult::DONE;
        if (button("Selesai", px + pw - 130, py + ph - 55, 100, 40,
                   Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
            // Pay pending rent if any
            if (bankruptcy_.pendingRent > 0 && bankruptcy_.rentOwner) {
                try {
                    player.deductCash(bankruptcy_.pendingRent);
                    bankruptcy_.rentOwner->addCash(bankruptcy_.pendingRent);
                } catch (const std::exception& e) {
                    showMessage("Pembayaran Gagal", e.what());
                }
            }
            dismiss();
        }
    } else if (bankruptcy_.options.empty()) {
        BankruptcyManager::declareBankrupt(*bankruptcy_.ctx);
        bankruptcy_.result = ActionResult::DONE;
        if (button("Bangkrut", px + pw / 2 - 60, py + ph - 55, 120, 40,
                   Color{244, 67, 54, 255}, WHITE, Color{255, 120, 100, 255})) {
            dismiss();
        }
    }
#endif
}

// ============================================================================
// UPGRADE
// ============================================================================

void PanelManager::renderUpgrade() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Upgrade Properti");

    if (!upgrade_.player || !upgrade_.ctx) { dismiss(); return; }

    if (upgrade_.eligible.empty()) {
        centeredText("Tidak ada properti yang dapat diupgrade.", static_cast<int>(py + 80), kFontBody, GRAY);
        if (button("Tutup", px + pw / 2 - 60, py + ph - 60, 120, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            upgrade_.result = ActionResult::CANCELLED;
            dismiss();
        }
        return;
    }

    float listY = py + 60;
    float itemH = 36;
    for (size_t i = 0; i < upgrade_.eligible.size(); ++i) {
        Property* prop = upgrade_.eligible[i];
        StreetProperty* sp = dynamic_cast<StreetProperty*>(prop);
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        std::string label = prop->getCode() + " (" + prop->getName() + ") - Level: " +
                            (sp ? std::to_string(sp->getBuildingCount()) : "0");
        DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 8), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            try {
                if (sp->getBuildingCount() < 4) {
                    sp->buildHouse();
                } else {
                    sp->upgradeToHotel();
                }
                upgrade_.result = ActionResult::DONE;
                dismiss();
            } catch (const std::exception& e) {
                showMessage("Upgrade Gagal", e.what());
            }
        }
    }
    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        upgrade_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// SKILL CARD
// ============================================================================

void PanelManager::renderSkillCard() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 350;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Gunakan Kartu");

    if (!skill_.player || !skill_.ctx) { dismiss(); return; }

    const auto& hand = skill_.player->getHand();
    if (hand.empty()) {
        centeredText("Tidak ada kartu di tangan.", static_cast<int>(py + 80), kFontBody, GRAY);
        if (button("Tutup", px + pw / 2 - 60, py + ph - 60, 120, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            skill_.result = ActionResult::CANCELLED;
            dismiss();
        }
        return;
    }

    float listY = py + 60;
    float itemH = 40;
    for (size_t i = 0; i < hand.size(); ++i) {
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        std::string label = std::to_string(i + 1) + ". " + hand[i]->getDescription();
        DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 10), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            try {
                skill_.player->useSCard(static_cast<int>(i + 1), *skill_.ctx);
                skill_.result = ActionResult::DONE;
                dismiss();
            } catch (const std::exception& e) {
                showMessage("Gagal", e.what());
            }
        }
    }
    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        skill_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// MORTGAGE
// ============================================================================

void PanelManager::renderMortgage() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Gadaikan Properti");

    if (!mortgage_.player) { dismiss(); return; }

    if (mortgage_.eligible.empty()) {
        centeredText("Tidak ada properti yang dapat digadaikan.", static_cast<int>(py + 80), kFontBody, GRAY);
        if (button("Tutup", px + pw / 2 - 60, py + ph - 60, 120, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            mortgage_.result = ActionResult::CANCELLED;
            dismiss();
        }
        return;
    }

    float listY = py + 60;
    float itemH = 36;
    for (size_t i = 0; i < mortgage_.eligible.size(); ++i) {
        Property* prop = mortgage_.eligible[i];
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        std::string label = prop->getCode() + " (" + prop->getName() + ") - Gadai: M" + std::to_string(prop->getMortgageValue());
        DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 8), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            try {
                mortgage_.player->mortgage(prop);
                mortgage_.result = ActionResult::DONE;
                dismiss();
            } catch (const std::exception& e) {
                showMessage("Gagal", e.what());
            }
        }
    }
    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        mortgage_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// DISMORTGAGE
// ============================================================================

void PanelManager::renderDismortgage() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Tebus Properti");

    if (!dismortgage_.player) { dismiss(); return; }

    if (dismortgage_.eligible.empty()) {
        centeredText("Tidak ada properti yang dapat ditebus.", static_cast<int>(py + 80), kFontBody, GRAY);
        if (button("Tutup", px + pw / 2 - 60, py + ph - 60, 120, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            dismortgage_.result = ActionResult::CANCELLED;
            dismiss();
        }
        return;
    }

    float listY = py + 60;
    float itemH = 36;
    for (size_t i = 0; i < dismortgage_.eligible.size(); ++i) {
        Property* prop = dismortgage_.eligible[i];
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        int cost = static_cast<int>(std::ceil(prop->getMortgageValue() * 1.1f));
        std::string label = prop->getCode() + " (" + prop->getName() + ") - Tebus: M" + std::to_string(cost);
        DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 8), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            try {
                dismortgage_.player->deductCash(cost);
                prop->redeem();
                dismortgage_.result = ActionResult::DONE;
                dismiss();
            } catch (const std::exception& e) {
                showMessage("Gagal", e.what());
            }
        }
    }
    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        dismortgage_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// SAVE
// ============================================================================

void PanelManager::renderSave() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 450, ph = 220;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Simpan Permainan");

    textInput_.update();
    textInput_.draw(px + 30, py + 70, pw - 60, 40, "namafile.txt");
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        textInput_.checkClick(px + 30, py + 70, pw - 60, 40);
    }

    if (button("Simpan", px + pw - 150, py + ph - 60, 110, 40,
               Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
        if (!textInput_.text.empty() && save_.engine) {
            save_.filename = textInput_.text;
            try {
                save_.engine->saveGame(save_.filename);
            } catch (const std::exception& e) {
                showMessage("Simpan Gagal", e.what());
            }
            save_.result = ActionResult::DONE;
            dismiss();
        }
    }
    if (button("Batal", px + 30, py + ph - 60, 100, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        save_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// PROPERTY CERT
// ============================================================================

void PanelManager::renderPropertyCert() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Akta Properti");

    textInput_.update();
    textInput_.draw(px + 30, py + 60, 200, 40, "Kode properti");
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        textInput_.checkClick(px + 30, py + 60, 200, 40);
    }

    if (button("Cari", px + 250, py + 60, 80, 40,
               Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
        cert_.propertyCode = textInput_.text;
    }

    if (!cert_.propertyCode.empty() && cert_.board) {
        Tile* tile = cert_.board->getTileByCode(cert_.propertyCode);
        PropertyTile* pt = dynamic_cast<PropertyTile*>(tile);
        if (pt && pt->getProperty()) {
            Property* p = pt->getProperty();
            std::string info = "Nama: " + p->getName() + "\n"
                               "Kode: " + p->getCode() + "\n"
                               "Harga Beli: M" + std::to_string(p->getBuyPrice()) + "\n"
                               "Nilai Gadai: M" + std::to_string(p->getMortgageValue());
            if (p->getStatus() == PropertyStatus::OWNED && p->getOwner()) {
                info += "\nPemilik: " + p->getOwner()->getUsername();
            } else if (p->getStatus() == PropertyStatus::BANK) {
                info += "\nPemilik: BANK";
            } else if (p->getStatus() == PropertyStatus::MORTGAGED) {
                info += "\nStatus: MORTGAGED";
            }
            if (StreetProperty* sp = dynamic_cast<StreetProperty*>(p)) {
                info += "\nRumah: " + std::to_string(sp->getBuildingCount()) +
                        "\nHotel: " + (sp->hasHotel() ? "Ya" : "Tidak");
            }
            wrappedText(info, static_cast<int>(px + 30), static_cast<int>(py + 120), pw - 60, kFontSmall, LIGHTGRAY, 22);
        } else {
            DrawText("Properti tidak ditemukan.", static_cast<int>(px + 30), static_cast<int>(py + 120), kFontBody, GRAY);
        }
    }

    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        cert_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// PROFILE
// ============================================================================

void PanelManager::renderProfile() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 450, ph = 350;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Profil Pemain");

    if (!profile_.player) { dismiss(); return; }

    Player* p = profile_.player;
    int houses = 0, hotels = 0;
    for (Property* prop : p->getProperties()) {
        if (StreetProperty* sp = dynamic_cast<StreetProperty*>(prop)) {
            if (sp->hasHotel()) ++hotels;
            else houses += sp->getBuildingCount();
        }
    }

    std::string info = "Nama: " + p->getUsername() + "\n"
                       "Saldo: M" + std::to_string(p->getBalance()) + "\n"
                       "Properti: " + std::to_string(p->getPropertiesAmount()) + "\n"
                       "Rumah: " + std::to_string(houses) + "\n"
                       "Hotel: " + std::to_string(hotels) + "\n"
                       "Kartu: " + std::to_string(p->getHandsAmount()) + "\n"
                       "Status: " + (p->getStatus() == PlayerStatus::ACTIVE ? "Aktif" :
                                     p->getStatus() == PlayerStatus::JAILED ? "Dipenjara" : "Bangkrut");
    wrappedText(info, static_cast<int>(px + 30), static_cast<int>(py + 60), pw - 60, kFontBody, LIGHTGRAY, 26);

    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        dismiss();
    }
#endif
}

// ============================================================================
// PROPERTIES LIST
// ============================================================================

void PanelManager::renderPropertiesList() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 450;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Daftar Properti");

    if (!properties_.player) { dismiss(); return; }

    const auto& props = properties_.player->getProperties();
    if (props.empty()) {
        centeredText("Tidak memiliki properti.", static_cast<int>(py + 80), kFontBody, GRAY);
    } else {
        float listY = py + 60;
        float itemH = 30;
        for (size_t i = 0; i < props.size(); ++i) {
            Property* p = props[i];
            float iy = listY + i * itemH;
            std::string status = (p->getStatus() == PropertyStatus::OWNED) ? "OWNED" :
                                 (p->getStatus() == PropertyStatus::MORTGAGED) ? "MORTGAGED" : "BANK";
            std::string label = std::to_string(i + 1) + ". " + p->getName() + " (" + p->getCode() + ") - " + status;
            DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 5), kFontSmall, LIGHTGRAY);
        }
    }

    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        dismiss();
    }
#endif
}

// ============================================================================
// LOG
// ============================================================================

void PanelManager::renderLog() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 600, ph = 500;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Log Transaksi");

    if (!log_.logger) { dismiss(); return; }

    const auto& entries = log_.logger->getAll();
    if (entries.empty()) {
        centeredText("Log kosong.", static_cast<int>(py + 80), kFontBody, GRAY);
    } else {
        float listY = py + 60;
        float itemH = 22;
        int start = std::max(0, static_cast<int>(entries.size()) - 18);
        for (int i = start; i < static_cast<int>(entries.size()); ++i) {
            float iy = listY + (i - start) * itemH;
            std::string line = entries[i].toString();
            DrawText(line.c_str(), static_cast<int>(px + 20), static_cast<int>(iy), 12, LIGHTGRAY);
        }
    }

    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        dismiss();
    }
#endif
}

// ============================================================================
// MESSAGE
// ============================================================================

void PanelManager::renderMessage() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 450, ph = 250;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, message_.title.c_str());

    wrappedText(message_.message, static_cast<int>(px + 30), static_cast<int>(py + 60), pw - 60, kFontBody, LIGHTGRAY, 24);

    if (button("OK", px + pw / 2 - 60, py + ph - 60, 120, 40,
               Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
        message_.acknowledged = true;
        dismiss();
    }
#endif
}

// ============================================================================
// FESTIVAL
// ============================================================================

void PanelManager::renderFestival() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 400;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Pilih Properti Festival");

    if (!festival_.player || !festival_.ctx) { dismiss(); return; }

    if (festival_.ownedProps.empty()) {
        centeredText("Tidak memiliki properti.", static_cast<int>(py + 80), kFontBody, GRAY);
        if (button("Tutup", px + pw / 2 - 60, py + ph - 60, 120, 40,
                   Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            festival_.result = ActionResult::CANCELLED;
            dismiss();
        }
        return;
    }

    float listY = py + 60;
    float itemH = 36;
    for (size_t i = 0; i < festival_.ownedProps.size(); ++i) {
        Property* prop = festival_.ownedProps[i];
        float iy = listY + i * itemH;
        Color bg = isMouseOver(px + 20, iy, pw - 40, itemH) ? Color{60, 60, 80, 255} : Color{45, 45, 60, 255};
        DrawRectangleRec(Rectangle{px + 20, iy, pw - 40.0f, itemH}, bg);
        std::string label = prop->getCode() + " (" + prop->getName() + ")";
        DrawText(label.c_str(), static_cast<int>(px + 30), static_cast<int>(iy + 8), kFontSmall, WHITE);
        if (isMouseOver(px + 20, iy, pw - 40, itemH) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            prop->applyFestival();
            festival_.result = ActionResult::DONE;
            dismiss();
        }
    }
    if (button("Tutup", px + pw / 2 - 60, py + ph - 55, 120, 40,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        festival_.result = ActionResult::CANCELLED;
        dismiss();
    }
#endif
}

// ============================================================================
// TAX
// ============================================================================

void PanelManager::renderTax() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 450, ph = 250;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    panelBg(pw, ph, "Pajak");

    if (!tax_.player) { dismiss(); return; }

    std::string info = "Pilih opsi pembayaran pajak:\n"
                       "1. Flat: M" + std::to_string(tax_.taxFlat) + "\n"
                       "2. " + std::to_string(static_cast<int>(tax_.taxPercent)) + "% dari kekayaan: M" +
                       std::to_string(static_cast<int>(std::ceil(tax_.taxPercent * tax_.player->getBalance() / 100.0f)));
    wrappedText(info, static_cast<int>(px + 30), static_cast<int>(py + 60), pw - 60, kFontBody, LIGHTGRAY, 24);

    std::string flatLabel = "Flat (M" + std::to_string(tax_.taxFlat) + ")";
    if (button(flatLabel.c_str(), px + 30, py + ph - 70, 180, 40,
               Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
        try {
            tax_.player->deductCash(tax_.taxFlat);
        } catch (const std::exception& e) {
            showMessage("Pajak Gagal", e.what());
        }
        tax_.result = ActionResult::DONE;
        dismiss();
    }
    int pctAmt = static_cast<int>(std::ceil(tax_.taxPercent * tax_.player->getBalance() / 100.0f));
    std::string pctLabel = "Persen (M" + std::to_string(pctAmt) + ")";
    if (button(pctLabel.c_str(), px + pw - 210, py + ph - 70, 180, 40,
               Color{255, 152, 0, 255}, WHITE, Color{255, 180, 80, 255})) {
        try {
            tax_.player->deductCash(pctAmt);
        } catch (const std::exception& e) {
            showMessage("Pajak Gagal", e.what());
        }
        tax_.result = ActionResult::DONE;
        dismiss();
    }
#endif
}

// ============================================================================
// CARD REVEAL
// ============================================================================

void PanelManager::renderCardReveal() {
#if NIMONSPOLI_HAS_RAYLIB
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = 500, ph = 350;
    float px = (sw - pw) / 2.0f;
    float py = (sh - ph) / 2.0f;
    
    std::string title = cardReveal_.cardType == "chance" ? "Kartu Kesempatan" : "Kartu Dana Umum";
    panelBg(pw, ph, title.c_str());
    
    wrappedText(cardReveal_.cardDescription, static_cast<int>(px + 30), static_cast<int>(py + 70), pw - 60, kFontBody, LIGHTGRAY, 26);
    
    if (button("Lanjutkan", px + pw / 2 - 80, py + ph - 70, 160, 45,
               Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
        if (cardReveal_.player && cardReveal_.ctx) {
            if (cardReveal_.chanceCard) {
                cardReveal_.chanceCard->execute(cardReveal_.player, *cardReveal_.ctx);
                cardReveal_.ctx->returnChanceCard(cardReveal_.chanceCard);
            } else if (cardReveal_.communityCard) {
                cardReveal_.communityCard->execute(cardReveal_.player, *cardReveal_.ctx);
                cardReveal_.ctx->returnCommunityChestCard(cardReveal_.communityCard);
            }
        }
        cardReveal_.acknowledged = true;
        cardReveal_.result = ActionResult::DONE;
        dismiss();
    }
#endif
}

} // namespace GuiPanels
