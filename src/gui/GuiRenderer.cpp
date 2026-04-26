#include "gui/GuiRenderer.hpp"

#include "gui/modules/GuiAssetModule.hpp"
#include "gui/modules/GuiLayoutModule.hpp"
#include "gui/modules/GuiVisualModule.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <sstream>

#if NIMONSPOLI_HAS_RAYLIB
#include <raymath.h>
#endif

namespace {
constexpr int kFontBody = 18;
constexpr int kFontSmall = 14;
constexpr int kFontTitle = 24;
constexpr int kFontLogo = 48;

int ClampDie(int value) {
    return std::max(1, std::min(6, value));
}

int GetTileSide(int index) {
    if (index >= 1 && index <= 10)
        return 0;
    if (index >= 11 && index <= 20)
        return 1;
    if (index >= 21 && index <= 30)
        return 2;
    if (index >= 31 && index <= 40)
        return 3;
    return 0;
}

float GetSideRotation(int side) {
    switch (side) {
    case 0:
        return 0.0f;
    case 1:
        return 90.0f;
    case 2:
        return 180.0f;
    case 3:
        return 270.0f;
    default:
        return 0.0f;
    }
}

bool IsCornerTileLocal(int index) {
    return (index == 1) || (index == 11) || (index == 21) || (index == 31);
}
} // namespace

GUIRenderer::GUIRenderer() {
    SeedDefaultTileOrder();
}

GUIRenderer::~GUIRenderer() {
    Shutdown();
}

bool GUIRenderer::Initialize() {
    const SetupConfig defaultConfig;
    return Initialize(defaultConfig);
}

bool GUIRenderer::Initialize(const SetupConfig& config) {
    ConfigureWindow(config.window.title,
                    config.window.width,
                    config.window.height,
                    config.window.resizable);

    if (!config.boardVisual.tileCodesInOrder.empty()) {
        SetTileOrder(config.boardVisual.tileCodesInOrder);
    }

    if (!config.boardVisual.tileTextureByCode.empty()) {
        SetTileVisualMap(config.boardVisual.tileTextureByCode);
    }

    const bool loaded = LoadAssets(config.assetRoot);

#if NIMONSPOLI_HAS_RAYLIB
    initialized_ = IsWindowReady();
#else
    initialized_ = true;
#endif

    return loaded;
}

void GUIRenderer::Shutdown() {
    UnloadAssets();

#if NIMONSPOLI_HAS_RAYLIB
    if (IsWindowReady()) {
        CloseWindow();
    }
#endif

    initialized_ = false;
}

bool GUIRenderer::IsInitialized() const {
#if NIMONSPOLI_HAS_RAYLIB
    return initialized_ && IsWindowReady();
#else
    return initialized_;
#endif
}

std::unordered_map<std::string, std::string>
GUIRenderer::BuildDefaultTileVisualMap() {
    return {
        {"go", "tile-go"},
        {"start", "tile-go"},
        {"mulai", "tile-go"},
        {"pen", "tile-jail"},
        {"jail", "tile-jail"},
        {"penjara", "tile-jail"},
        {"bbp", "tile-free-parking"},
        {"free-parking", "tile-free-parking"},
        {"bebas parkir", "tile-free-parking"},
        {"ppj", "tile-go-to-jail"},
        {"gotojail", "tile-go-to-jail"},
        {"pergi ke penjara", "tile-go-to-jail"},
        {"stasiun", "tile-stasiun"},
        {"railroad", "tile-stasiun"},
        {"rr", "tile-stasiun"},
        {"pln", "tile-pln"},
        {"pam", "tile-pam"},
        {"festival", "tile-festival"},
        {"kesempatan", "tile-kesempatan"},
        {"chance", "tile-kesempatan"},
        {"danaumum", "tile-kesempatan"},
        {"dana-umum", "tile-kesempatan"},
        {"pph", "tile-tax"},
        {"pbm", "tile-tax"},
        {"pajak", "tile-tax"},
        {"tax", "tile-tax"},
    };
}

void GUIRenderer::ConfigureWindow(const std::string& title,
                                  const int width,
                                  const int height,
                                  const bool resizable) {
    screenWidth_ = width;
    screenHeight_ = height;

#if NIMONSPOLI_HAS_RAYLIB
    int flags = FLAG_MSAA_4X_HINT;
    if (resizable)
        flags |= FLAG_WINDOW_RESIZABLE;
    SetConfigFlags(flags);

    if (!IsWindowReady()) {
        InitWindow(width, height, title.c_str());
        SetTargetFPS(60);
    } else {
        SetWindowSize(width, height);
        SetWindowTitle(title.c_str());
    }
#else
    (void) title;
    (void) width;
    (void) height;
    (void) resizable;
#endif
}

bool GUIRenderer::LoadAssets(const std::string& assetRoot) {
    return GuiAssetModule::LoadAll(textures_, assetsLoaded_, assetRoot);
}

bool GUIRenderer::LoadSingleAsset(const std::string& assetRoot,
                                  const std::string& key,
                                  const std::string& fileName) {
    return GuiAssetModule::LoadSingle(textures_, assetRoot, key, fileName);
}

void GUIRenderer::UnloadAssets() {
    GuiAssetModule::UnloadAll(textures_, assetsLoaded_);
}

bool GUIRenderer::IsAssetReady(const std::string& key) const {
    return HasTexture(key);
}

void GUIRenderer::SetTileOrder(
    const std::vector<std::string>& tileCodesInOrder) {
    if (tileCodesInOrder.size() != kBoardTileCount)
        return;

    tileCodesByIndex_.assign(kBoardTileCount + 1, "");
    tileIndexByCode_.clear();

    for (int i = 1; i <= kBoardTileCount; ++i) {
        tileCodesByIndex_[i] =
            tileCodesInOrder[static_cast<std::size_t>(i - 1)];
        tileIndexByCode_[ToLower(tileCodesByIndex_[i])] = i;
    }
}

void GUIRenderer::SetTileVisualMap(
    const std::unordered_map<std::string, std::string>& tileTextureByCode) {
    tileTextureByCode_.clear();
    for (const auto& entry : tileTextureByCode) {
        tileTextureByCode_[ToLower(entry.first)] = entry.second;
    }
}

void GUIRenderer::SetDiceValues(const int die1, const int die2) {
    die1_ = ClampDie(die1);
    die2_ = ClampDie(die2);
    diceAnim_.isRolling = false;
    diceAnim_.displayDie1 = die1_;
    diceAnim_.displayDie2 = die2_;
}

void GUIRenderer::StartDiceRoll(int finalDie1, int finalDie2) {
    diceAnim_.finalDie1 = ClampDie(finalDie1);
    diceAnim_.finalDie2 = ClampDie(finalDie2);
    diceAnim_.isRolling = true;
    diceAnim_.currentFrame = 0;
}

void GUIRenderer::UpdateDiceAnimation() {
    if (!diceAnim_.isRolling)
        return;

    diceAnim_.currentFrame++;

    if (diceAnim_.currentFrame % DiceAnimationState::kFrameInterval == 0) {
        diceAnim_.displayDie1 = (std::rand() % 6) + 1;
        diceAnim_.displayDie2 = (std::rand() % 6) + 1;
    }

    if (diceAnim_.currentFrame >= DiceAnimationState::kRollDuration) {
        diceAnim_.isRolling = false;
        diceAnim_.displayDie1 = diceAnim_.finalDie1;
        diceAnim_.displayDie2 = diceAnim_.finalDie2;
        die1_ = diceAnim_.finalDie1;
        die2_ = diceAnim_.finalDie2;
    }
}

void GUIRenderer::StartTokenHop(int playerIndex, int fromTile, int toTile) {
    tokenHopAnim_.isActive = true;
    tokenHopAnim_.playerIndex = playerIndex;
    tokenHopAnim_.fromTileIndex = fromTile;
    tokenHopAnim_.currentTileIndex = fromTile;
    tokenHopAnim_.toTileIndex = toTile;
    tokenHopAnim_.hopProgress = 0.0f;
}

void GUIRenderer::UpdateTokenAnimation() {
    if (!tokenHopAnim_.isActive)
        return;

    float dt = 0.016f;
#if NIMONSPOLI_HAS_RAYLIB
    dt = GetFrameTime();
#endif

    tokenHopAnim_.hopProgress += TokenHopAnimation::kHopSpeed * dt;

    if (tokenHopAnim_.hopProgress >= 1.0f) {
        // Finished hopping to currentTileIndex (which is the "next" tile)
        tokenHopAnim_.fromTileIndex = tokenHopAnim_.currentTileIndex;

        if (tokenHopAnim_.fromTileIndex == tokenHopAnim_.toTileIndex) {
            // Arrived at final destination
            tokenHopAnim_.isActive = false;
            return;
        }

        // Advance to next tile
        int next = tokenHopAnim_.fromTileIndex + 1;
        if (next > kBoardTileCount) next = 1;
        tokenHopAnim_.currentTileIndex = next;
        tokenHopAnim_.hopProgress = 0.0f;
    }
}

bool GUIRenderer::IsTokenAnimating() const {
    return tokenHopAnim_.isActive;
}

void GUIRenderer::ResetTokenAnimation() {
    tokenHopAnim_.isActive = false;
}

GUIRenderer::Square GUIRenderer::GetTileSquare(const int index) const {
    return GuiLayoutModule::TileSquare(index, tileSize_, boardBounds_);
}

int GUIRenderer::ResolveTileIndexFromCode(const std::string& code) const {
    return GuiLayoutModule::ResolveTileIndex(tileIndexByCode_, code);
}

GUIRenderer::BoardDisplaySnapshot GUIRenderer::InspectBoardDisplay() const {
    BoardDisplaySnapshot snapshot;

#if NIMONSPOLI_HAS_RAYLIB
    const int sw = IsWindowReady() ? GetScreenWidth() : screenWidth_;
    const int sh = IsWindowReady() ? GetScreenHeight() : screenHeight_;
#else
    const int sw = screenWidth_;
    const int sh = screenHeight_;
#endif

    GuiLayoutModule::Recalculate(snapshot.boardBounds,
                                 snapshot.centerArea,
                                 snapshot.rightPanel,
                                 snapshot.bottomPanel,
                                 snapshot.tileSize,
                                 sw,
                                 sh,
                                 kBoardMargin,
                                 kRightPanelWidth,
                                 kBottomPanelHeight);

    snapshot.tileSquares.reserve(kBoardTileCount);
    for (int i = 1; i <= kBoardTileCount; ++i) {
        snapshot.tileSquares.push_back(GuiLayoutModule::TileSquare(
            i, snapshot.tileSize, snapshot.boardBounds));
    }

    return snapshot;
}

void GUIRenderer::DrawBoard(const GameState& state) {
    if (!IsInitialized())
        return;

    RecalculateLayout();
    UpdateDiceAnimation();
    UpdateTokenAnimation();

#if NIMONSPOLI_HAS_RAYLIB
    DrawPanelsBackdrop();
    DrawBaseTiles(state);
    DrawTileOverlays(state);
    DrawBuildings(state);
    DrawTokens(state);
    DrawCenterUI(state);
    DrawPlayerPanels(state);
    DrawDiceOutsideBoard();
#else
    (void) state;
#endif
}

void GUIRenderer::SeedDefaultTileOrder() {
    tileCodesByIndex_.assign(kBoardTileCount + 1, "");
    tileIndexByCode_.clear();

    for (int i = 1; i <= kBoardTileCount; ++i) {
        std::ostringstream oss;
        oss << "T" << (i < 10 ? "0" : "") << i;
        tileCodesByIndex_[i] = oss.str();
        tileIndexByCode_[ToLower(tileCodesByIndex_[i])] = i;
    }
}

void GUIRenderer::RecalculateLayout() {
#if NIMONSPOLI_HAS_RAYLIB
    const int sw = IsWindowReady() ? GetScreenWidth() : screenWidth_;
    const int sh = IsWindowReady() ? GetScreenHeight() : screenHeight_;
#else
    const int sw = screenWidth_;
    const int sh = screenHeight_;
#endif

    GuiLayoutModule::Recalculate(boardBounds_,
                                 centerArea_,
                                 rightPanel_,
                                 bottomPanel_,
                                 tileSize_,
                                 sw,
                                 sh,
                                 kBoardMargin,
                                 kRightPanelWidth,
                                 kBottomPanelHeight);
}

void GUIRenderer::DrawPanelsBackdrop() const {
#if NIMONSPOLI_HAS_RAYLIB
    DrawRectangleRec(boardBounds_, Color{234, 224, 205, 255});
    DrawRectangleLinesEx(boardBounds_, 4.0F, Color{50, 35, 20, 255});

    DrawRectangleRec(centerArea_, Color{248, 244, 235, 255});
    DrawRectangleLinesEx(centerArea_, 2.0F, Color{100, 85, 65, 255});

    DrawRectangleRec(rightPanel_, Color{247, 241, 229, 255});
    DrawRectangleLinesEx(rightPanel_, 2.0F, Color{80, 70, 55, 255});

    DrawRectangleRec(bottomPanel_, Color{247, 241, 229, 255});
    DrawRectangleLinesEx(bottomPanel_, 2.0F, Color{80, 70, 55, 255});
#endif
}

void GUIRenderer::DrawBaseTiles(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    for (int i = 1; i <= kBoardTileCount; ++i) {
        DrawTileBase(i, state);
    }
#endif
}

void GUIRenderer::DrawTileBase(const int index, const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    const Square square = GetTileSquare(index);
    Rectangle tileRect = square.asRectangle();

    const std::string& code = tileCodesByIndex_[index];
    const PropertyState* property = FindPropertyStateByCode(state, code);
    const std::string textureKey = GetTileTextureKey(index, property);

    bool isStreet = IsStreetTile(index, property);
    bool isCorner = IsCornerTile(index);

    auto tileNameIt = state.tileNames.find(index - 1);
    std::string tileName = (tileNameIt != state.tileNames.end()) ? tileNameIt->second : "";

    Color tileBg;
    if (isCorner) {
        tileBg = Color{205, 197, 178, 255};
    } else if (isStreet) {
        Color headerColor = GetColorGroupForTile(index, property);
        DrawRotatedTileContent(
            square.side, tileRect, headerColor, code, property, textureKey);
        DrawRectangleLinesEx(tileRect, 2.0F, Color{50, 40, 30, 255});
        return;
    } else if (property != nullptr) {
        std::string typeLower = ToLower(property->type);
        if (typeLower.find("railroad") != std::string::npos) {
            tileBg = Color{220, 230, 240, 255};
        } else if (typeLower.find("utility") != std::string::npos) {
            tileBg = Color{230, 245, 220, 255};
        } else {
            tileBg = Color{240, 235, 220, 255};
        }
    } else {
        std::string lowerCode = ToLower(code);
        if (lowerCode.find("dnu") != std::string::npos ||
            lowerCode.find("dana") != std::string::npos ||
            lowerCode.find("community") != std::string::npos ||
            lowerCode.find("ksp") != std::string::npos ||
            lowerCode.find("kesempatan") != std::string::npos ||
            lowerCode.find("chance") != std::string::npos) {
            tileBg = Color{200, 215, 240, 255};
        } else if (lowerCode.find("fes") != std::string::npos ||
                   lowerCode.find("festival") != std::string::npos) {
            tileBg = Color{245, 230, 200, 255};
        } else if (lowerCode.find("pph") != std::string::npos ||
                   lowerCode.find("pbm") != std::string::npos ||
                   lowerCode.find("tax") != std::string::npos ||
                   lowerCode.find("pajak") != std::string::npos) {
            tileBg = Color{240, 210, 210, 255};
        } else if (lowerCode.find("go") != std::string::npos ||
                   lowerCode.find("mulai") != std::string::npos) {
            tileBg = Color{200, 230, 200, 255};
        } else if (lowerCode.find("pen") != std::string::npos ||
                   lowerCode.find("jail") != std::string::npos) {
            tileBg = Color{220, 210, 195, 255};
        } else if (lowerCode.find("bbp") != std::string::npos ||
                   lowerCode.find("free") != std::string::npos ||
                   lowerCode.find("parkir") != std::string::npos) {
            tileBg = Color{210, 230, 210, 255};
        } else if (lowerCode.find("ppj") != std::string::npos ||
                   lowerCode.find("gotojail") != std::string::npos) {
            tileBg = Color{230, 200, 200, 255};
        } else {
            tileBg = Color{240, 235, 220, 255};
        }
    }

    DrawRectangleRec(tileRect, tileBg);
    DrawRectangleLinesEx(tileRect, 2.0F, Color{50, 40, 30, 255});

    if (!textureKey.empty()) {
        Rectangle textureDst = Rectangle{tileRect.x + 4.0F,
                                          tileRect.y + 4.0F,
                                          tileRect.width - 8.0F,
                                          tileRect.height - 8.0F};
        DrawTextureFitted(textureKey, textureDst);
    }

    if (!tileName.empty()) {
        int nameFontSize = kFontSmall;
        int nameW = MeasureText(tileName.c_str(), nameFontSize);
        int availW = static_cast<int>(tileRect.width - 8.0F);
        if (nameW > availW && nameFontSize > 8) {
            nameFontSize = std::max(8, nameFontSize * availW / nameW);
            nameW = MeasureText(tileName.c_str(), nameFontSize);
        }
        DrawText(tileName.c_str(),
                 static_cast<int>(tileRect.x + (tileRect.width - nameW) * 0.5f),
                 static_cast<int>(tileRect.y + tileRect.height - nameFontSize - 4.0f),
                 nameFontSize,
                 Color{40, 40, 40, 255});
    }

    DrawText(TextFormat("%d", index),
             static_cast<int>(tileRect.x + 3.0F),
             static_cast<int>(tileRect.y + 3.0F),
             kFontSmall,
             Color{30, 30, 30, 180});
#endif
}

void GUIRenderer::DrawRotatedTileContent(int side,
                                         const Rectangle& tileRect,
                                         const Color& headerColor,
                                         const std::string& code,
                                         const PropertyState* property,
                                         const std::string& textureKey) const {
#if NIMONSPOLI_HAS_RAYLIB
    float rotation = GetSideRotation(side);
    float centerX = tileRect.x + tileRect.width * 0.5f;
    float centerY = tileRect.y + tileRect.height * 0.5f;

    rlPushMatrix();
    rlTranslatef(centerX, centerY, 0.0f);
    rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
    rlTranslatef(-centerX, -centerY, 0.0f);

    DrawRectangleLinesEx(tileRect, 2.0F, Color{50, 40, 30, 255});

    float headerHeight = tileRect.height * kTileHeaderRatio;
    Rectangle headerRect =
        Rectangle{tileRect.x, tileRect.y, tileRect.width, headerHeight};
    DrawRectangleRec(headerRect, headerColor);

    Rectangle bodyRect = Rectangle{tileRect.x,
                                   tileRect.y + headerHeight,
                                   tileRect.width,
                                   tileRect.height - headerHeight};
    DrawRectangleRec(bodyRect, Color{252, 250, 245, 255});

    if (!textureKey.empty()) {
        float iconSize = tileRect.width * 0.5f;
        Rectangle iconDst =
            Rectangle{tileRect.x + (tileRect.width - iconSize) * 0.5f,
                      bodyRect.y + (bodyRect.height - iconSize) * 0.5f,
                      iconSize,
                      iconSize};
        DrawTextureFitted(textureKey, iconDst);
    }

    const char* label = TextFormat("%s", code.c_str());
    int textWidth = MeasureText(label, kFontSmall);
    DrawText(label,
             static_cast<int>(tileRect.x + (tileRect.width - textWidth) * 0.5f),
             static_cast<int>(bodyRect.y + 4.0f),
             kFontSmall,
             Color{40, 40, 40, 255});

    rlPopMatrix();
#endif
}

void GUIRenderer::DrawTileOverlays(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    for (const PropertyState& property : state.properties) {
        DrawTileOverlayForProperty(property);
    }
#endif
}

void GUIRenderer::DrawTileOverlayForProperty(
    const PropertyState& property) const {
#if NIMONSPOLI_HAS_RAYLIB
    const int idx = ResolveTileIndexFromCode(property.code);
    if (idx < 1 || idx > kBoardTileCount)
        return;

    const Rectangle dst = GetTileSquare(idx).asRectangle();
    const Color overlayTint =
        Color{255,
              255,
              255,
              static_cast<unsigned char>(255.0F * kOverlayAlphaFactor)};

    if (ContainsInsensitive(property.status, "mortgage") ||
        ContainsInsensitive(property.status, "gadai")) {
        if (HasTexture("overlay-mortgage")) {
            DrawTextureFitted("overlay-mortgage", dst, overlayTint);
        } else {
            DrawRectangleRec(dst, Color{140, 140, 140, 80});
        }
    }

    if (property.festivalDur > 0 || property.festivalMult > 1) {
        if (HasTexture("overlay-festival")) {
            DrawTextureFitted("overlay-festival", dst, overlayTint);
        } else {
            DrawRectangleRec(dst, Color{255, 120, 60, 80});
        }
    }
#endif
}

void GUIRenderer::DrawBuildings(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    for (const PropertyState& property : state.properties) {
        DrawBuildingForProperty(property);
    }
#endif
}

void GUIRenderer::DrawBuildingForProperty(const PropertyState& property) const {
#if NIMONSPOLI_HAS_RAYLIB
    const int idx = ResolveTileIndexFromCode(property.code);
    if (idx < 1 || idx > kBoardTileCount)
        return;

    const Square square = GetTileSquare(idx);
    float headerHeight = square.size * kTileHeaderRatio;
    float buildingY = square.y + headerHeight + 2.0f;

    if (property.isHotel) {
        float size = square.size * 0.4f;
        Rectangle dst = Rectangle{square.x + (square.size - size) * 0.5f,
                                  buildingY,
                                  size,
                                  size * 0.8f};
        if (HasTexture("building-hotel")) {
            DrawTextureFitted("building-hotel", dst);
        } else {
            DrawRectangleRec(dst, Color{196, 32, 52, 255});
        }
        return;
    }

    int houseCount = std::max(0, std::min(4, property.buildingCount));
    if (houseCount <= 0)
        return;

    float slotWidth = square.size / 4.0f;
    for (int h = 0; h < houseCount; ++h) {
        Rectangle dst = Rectangle{square.x + slotWidth * h + 2.0f,
                                  buildingY,
                                  slotWidth - 4.0f,
                                  square.size * 0.25f};
        if (HasTexture("building-house")) {
            DrawTextureFitted("building-house", dst);
        } else {
            DrawRectangleRec(dst, Color{26, 155, 80, 255});
        }
    }
#endif
}

void GUIRenderer::DrawTokens(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    std::array<Vector2, 4> stackOffset = {
        Vector2{-8.0f, -8.0f},
        Vector2{8.0f, -8.0f},
        Vector2{-8.0f, 8.0f},
        Vector2{8.0f, 8.0f},
    };

    std::vector<int> occupancy(kBoardTileCount + 1, 0);

    for (std::size_t i = 0; i < state.players.size(); ++i) {
        const PlayerState& player = state.players[i];
        int tileIndex = ResolveTileIndexFromCode(player.positionCode);
        if (tileIndex < 1 || tileIndex > kBoardTileCount)
            continue;

        float hopOffsetY = 0.0f;
        bool isHopping = (tokenHopAnim_.isActive &&
                          static_cast<int>(i) == tokenHopAnim_.playerIndex);

        if (isHopping) {
            int fromIdx = tokenHopAnim_.fromTileIndex;
            int curIdx = tokenHopAnim_.currentTileIndex;
            float progress = tokenHopAnim_.hopProgress;

            Square fromSquare = GetTileSquare(fromIdx);
            Square toSquare = GetTileSquare(curIdx);
            Vector2 fromCenter = Vector2{fromSquare.x + fromSquare.size * 0.5f,
                                          fromSquare.y + fromSquare.size * 0.5f};
            Vector2 toCenter = Vector2{toSquare.x + toSquare.size * 0.5f,
                                       toSquare.y + toSquare.size * 0.5f};

            float drawX = fromCenter.x + (toCenter.x - fromCenter.x) * progress;
            float drawY = fromCenter.y + (toCenter.y - fromCenter.y) * progress;
            hopOffsetY = -TokenHopAnimation::kHopHeight *
                          sinf(progress * 3.14159265f);

            const int stackIdx = std::min(3, occupancy[curIdx]);
            occupancy[curIdx]++;
            const Vector2 off = stackOffset[static_cast<std::size_t>(stackIdx)];
            const float tokenSize = fromSquare.size * kTokenScale;

            const Rectangle dst = Rectangle{drawX - tokenSize * 0.5f + off.x,
                                            drawY + hopOffsetY - tokenSize * 0.5f + off.y,
                                            tokenSize,
                                            tokenSize};

            const std::string key = "player" + std::to_string((i % 4) + 1);
            if (HasTexture(key)) {
                const Texture2D tex = GetTextureOrEmpty(key);
                DrawTexturePro(tex,
                               Rectangle{0.0f, 0.0f,
                                         static_cast<float>(tex.width),
                                         static_cast<float>(tex.height)},
                               dst, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
            } else {
                DrawCircleV(Vector2{drawX + off.x, drawY + hopOffsetY + off.y},
                            tokenSize * 0.25f, Color{35, 96, 190, 255});
            }
            continue;
        }

        const int stackIndex = std::min(3, occupancy[tileIndex]);
        occupancy[tileIndex]++;

        const Square square = GetTileSquare(tileIndex);
        const float tokenSize = square.size * kTokenScale;
        const Vector2 center = Vector2{square.x + square.size * 0.5f,
                                       square.y + square.size * 0.5f};
        const Vector2 off = stackOffset[static_cast<std::size_t>(stackIndex)];

        const Rectangle dst = Rectangle{center.x - tokenSize * 0.5f + off.x,
                                        center.y - tokenSize * 0.5f + off.y,
                                        tokenSize,
                                        tokenSize};

        const std::string key = "player" + std::to_string((i % 4) + 1);
        if (HasTexture(key)) {
            const Texture2D tex = GetTextureOrEmpty(key);
            DrawTexturePro(tex,
                           Rectangle{0.0f,
                                     0.0f,
                                     static_cast<float>(tex.width),
                                     static_cast<float>(tex.height)},
                           dst,
                           Vector2{0.0f, 0.0f},
                           0.0f,
                           WHITE);
        } else {
            DrawCircleV(center, tokenSize * 0.25f, Color{35, 96, 190, 255});
        }
    }
#endif
}

void GUIRenderer::DrawCenterUI(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    DrawCenterLogo();
    DrawDeckStacks();
    DrawTransactionLog(state);
#endif
}

void GUIRenderer::DrawCenterLogo() const {
#if NIMONSPOLI_HAS_RAYLIB
    float centerX = centerArea_.x + centerArea_.width * 0.5f;
    float centerY = centerArea_.y + centerArea_.height * 0.5f;

    // Draw nimonspoli.png image rotated -45 degrees
    const std::string logoKey = "nimonspoli";

    if (HasTexture(logoKey)) {
        const Texture2D tex = GetTextureOrEmpty(logoKey);

        // Calculate size to fit within center area (max 60% of center area)
        float maxSize = std::min(centerArea_.width, centerArea_.height) * 0.6f;
        float scale = std::min(maxSize / tex.width, maxSize / tex.height);

        float imgWidth = tex.width * scale;
        float imgHeight = tex.height * scale;

        Rectangle src = Rectangle{0.0f,
                                  0.0f,
                                  static_cast<float>(tex.width),
                                  static_cast<float>(tex.height)};
        Rectangle dst = Rectangle{centerX - imgWidth * 0.5f,
                                  centerY - imgHeight * 0.5f,
                                  imgWidth,
                                  imgHeight};
        Vector2 origin = Vector2{imgWidth * 0.5f, imgHeight * 0.5f};

        // Draw rotated -45 degrees
        DrawTexturePro(tex,
                       src,
                       Rectangle{centerX, centerY, imgWidth, imgHeight},
                       origin,
                       -45.0f,
                       WHITE);
    } else {
        // Fallback to text if image not found
        const char* logoText = "NIMONSPOLI";
        float fontSize = kFontLogo;
        int textWidth = MeasureText(logoText, static_cast<int>(fontSize));
        int textHeight = static_cast<int>(fontSize);

        rlPushMatrix();
        rlTranslatef(centerX, centerY, 0.0f);
        rlRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
        rlTranslatef(-centerX, -centerY, 0.0f);

        DrawRectangle(static_cast<int>(centerX - textWidth * 0.5f - 20),
                      static_cast<int>(centerY - textHeight * 0.5f - 10),
                      textWidth + 40,
                      textHeight + 20,
                      Color{200, 50, 50, 255});

        DrawText(logoText,
                 static_cast<int>(centerX - textWidth * 0.5f),
                 static_cast<int>(centerY - textHeight * 0.5f + 5),
                 static_cast<int>(fontSize),
                 WHITE);

        rlPopMatrix();
    }
#endif
}

void GUIRenderer::DrawDeckStacks() const {
#if NIMONSPOLI_HAS_RAYLIB
    // Card aspect ratio: 243:157 (landscape)
    constexpr float cardAspectRatio = 243.0f / 157.0f; // ~1.55

    // Calculate card size maintaining aspect ratio
    float maxCardWidth = centerArea_.width * 0.22f;
    float cardWidth = maxCardWidth;
    float cardHeight = cardWidth / cardAspectRatio;

    // Dana Umum at top-left of center area
    float danaX = centerArea_.x + centerArea_.width * 0.08f;
    float danaY = centerArea_.y + centerArea_.height * 0.08f;
    Rectangle danaDst = Rectangle{danaX, danaY, cardWidth, cardHeight};

    // Rotate -45 degrees (aligned with nimonspoli diagonal)
    rlPushMatrix();
    rlTranslatef(
        danaDst.x + cardWidth * 0.5f, danaDst.y + cardHeight * 0.5f, 0.0f);
    rlRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
    rlTranslatef(-(danaDst.x + cardWidth * 0.5f),
                 -(danaDst.y + cardHeight * 0.5f),
                 0.0f);

    if (HasTexture("deck-dana-umum")) {
        DrawTextureFitted("deck-dana-umum", danaDst);
    } else if (HasTexture("kartu-dana-umum")) {
        DrawTextureFitted("kartu-dana-umum", danaDst);
    } else {
        DrawRectangleRec(danaDst, Color{236, 220, 170, 255});
        DrawRectangleLinesEx(danaDst, 2.0f, Color{180, 160, 100, 255});
        DrawText("DANA UMUM",
                 static_cast<int>(danaDst.x + cardWidth * 0.15f),
                 static_cast<int>(danaDst.y + cardHeight * 0.4f),
                 kFontSmall,
                 Color{80, 60, 30, 255});
    }
    rlPopMatrix();

    // Kesempatan at bottom-right of center area
    float kesempatanX = centerArea_.x + centerArea_.width * 0.70f;
    float kesempatanY = centerArea_.y + centerArea_.height * 0.65f;
    Rectangle chanceDst =
        Rectangle{kesempatanX, kesempatanY, cardWidth, cardHeight};

    // Rotate -45 degrees (same axis as nimonspoli)
    rlPushMatrix();
    rlTranslatef(
        chanceDst.x + cardWidth * 0.5f, chanceDst.y + cardHeight * 0.5f, 0.0f);
    rlRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
    rlTranslatef(-(chanceDst.x + cardWidth * 0.5f),
                 -(chanceDst.y + cardHeight * 0.5f),
                 0.0f);

    if (HasTexture("deck-kesempatan")) {
        DrawTextureFitted("deck-kesempatan", chanceDst);
    } else if (HasTexture("kartu-kesempatan")) {
        DrawTextureFitted("kartu-kesempatan", chanceDst);
    } else {
        DrawRectangleRec(chanceDst, Color{172, 212, 240, 255});
        DrawRectangleLinesEx(chanceDst, 2.0f, Color{100, 140, 180, 255});
        DrawText("KESEMPATAN",
                 static_cast<int>(chanceDst.x + cardWidth * 0.15f),
                 static_cast<int>(chanceDst.y + cardHeight * 0.45f),
                 kFontSmall,
                 Color{30, 50, 80, 255});
    }
    rlPopMatrix();
#endif
}

void GUIRenderer::DrawDiceOutsideBoard() {
#if NIMONSPOLI_HAS_RAYLIB
    float dieSize = 56.0f;
    float diceAreaY = rightPanel_.y + 310.0f;
    float diceSpacing = 10.0f;

    float panelCenterX = rightPanel_.x + rightPanel_.width * 0.5f;
    float die1X = panelCenterX - dieSize - diceSpacing * 0.5f;
    float die2X = panelCenterX + diceSpacing * 0.5f;

    int display1 = diceAnim_.isRolling ? diceAnim_.displayDie1 : die1_;
    int display2 = diceAnim_.isRolling ? diceAnim_.displayDie2 : die2_;

    const std::string dieKey1 = "dice" + std::to_string(display1);
    const std::string dieKey2 = "dice" + std::to_string(display2);

    Rectangle die1Dst = Rectangle{die1X, diceAreaY, dieSize, dieSize};
    Rectangle die2Dst = Rectangle{die2X, diceAreaY, dieSize, dieSize};

    // Draw shadow
    DrawRectangleRec(Rectangle{die1Dst.x + 4, die1Dst.y + 4, dieSize, dieSize},
                     Color{0, 0, 0, 60});
    DrawRectangleRec(Rectangle{die2Dst.x + 4, die2Dst.y + 4, dieSize, dieSize},
                     Color{0, 0, 0, 60});

    // Draw dice background (white square)
    DrawRectangleRec(die1Dst, Color{255, 255, 255, 255});
    DrawRectangleRec(die2Dst, Color{255, 255, 255, 255});

    // Draw dice texture or fallback
    if (HasTexture(dieKey1)) {
        DrawTextureFitted(dieKey1, die1Dst);
    } else {
        DrawRectangleLinesEx(die1Dst, 2.0f, Color{80, 80, 80, 255});
        DrawText(TextFormat("%d", display1),
                 static_cast<int>(die1Dst.x + dieSize * 0.35f),
                 static_cast<int>(die1Dst.y + dieSize * 0.3f),
                 kFontBody,
                 BLACK);
    }

    if (HasTexture(dieKey2)) {
        DrawTextureFitted(dieKey2, die2Dst);
    } else {
        DrawRectangleLinesEx(die2Dst, 2.0f, Color{80, 80, 80, 255});
        DrawText(TextFormat("%d", display2),
                 static_cast<int>(die2Dst.x + dieSize * 0.35f),
                 static_cast<int>(die2Dst.y + dieSize * 0.3f),
                 kFontBody,
                 BLACK);
    }

    // Draw border around dice
    DrawRectangleLinesEx(die1Dst, 2.0f, Color{60, 60, 60, 255});
    DrawRectangleLinesEx(die2Dst, 2.0f, Color{60, 60, 60, 255});

    // Draw "DICE" label
    const char* label = diceAnim_.isRolling ? "ROLLING..." : "DICE";
    int labelWidth = MeasureText(label, kFontSmall);
    DrawText(label,
             static_cast<int>(panelCenterX - labelWidth * 0.5f),
             static_cast<int>(diceAreaY + dieSize + 8),
             kFontSmall,
             diceAnim_.isRolling ? Color{200, 50, 50, 255}
                                 : Color{80, 80, 80, 255});
#endif
}

void GUIRenderer::DrawTransactionLog(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    float logX = centerArea_.x + 20.0f;
    float logY = centerArea_.y + centerArea_.height - 140.0f;
    float logW = centerArea_.width - 40.0f;

    DrawRectangleRec(Rectangle{logX, logY, logW, 120.0f},
                     Color{255, 255, 255, 200});
    DrawRectangleLinesEx(
        Rectangle{logX, logY, logW, 120.0f}, 1.0f, Color{150, 150, 150, 255});

    DrawText("Transaction Log",
             static_cast<int>(logX + 10),
             static_cast<int>(logY + 5),
             kFontBody,
             Color{50, 45, 35, 255});

    const int logStart = std::max(0, static_cast<int>(state.log.size()) - 5);
    int line = 0;
    for (int i = logStart; i < static_cast<int>(state.log.size()); ++i) {
        const std::string text =
            state.log[static_cast<std::size_t>(i)].toString();
        DrawText(text.c_str(),
                 static_cast<int>(logX + 10),
                 static_cast<int>(logY + 30 + line * 18),
                 kFontSmall,
                 Color{60, 56, 48, 255});
        ++line;
    }
#endif
}

void GUIRenderer::DrawPlayerPanels(const GameState& state) const {
#if NIMONSPOLI_HAS_RAYLIB
    DrawText("Player Status",
             static_cast<int>(rightPanel_.x + 12.0f),
             static_cast<int>(rightPanel_.y + 10.0f),
             kFontTitle,
             Color{45, 38, 30, 255});

    int y = static_cast<int>(rightPanel_.y + 40.0f);

    for (std::size_t i = 0; i < state.players.size(); ++i) {
        const PlayerState& p = state.players[i];
        const bool isActive = static_cast<int>(i) == state.activePlayerIdx;

        const int ownedPropertyCount = static_cast<int>(
            std::count_if(state.properties.begin(),
                          state.properties.end(),
                          [&p](const PropertyState& prop) {
                              return ToLower(prop.owner) == ToLower(p.username);
                          }));

        DrawSinglePlayerStatusRow(
            p, static_cast<int>(i), y, ownedPropertyCount, isActive);

        y += 55;
        if (y > static_cast<int>(rightPanel_.y + rightPanel_.height - 80.0f)) {
            break;
        }
    }

    DrawText("Active Hand Cards",
             static_cast<int>(bottomPanel_.x + 12.0f),
             static_cast<int>(bottomPanel_.y + 8.0f),
             kFontBody,
             Color{45, 38, 30, 255});

    if (state.activePlayerIdx < 0 ||
        state.activePlayerIdx >= static_cast<int>(state.players.size())) {
        return;
    }

    const PlayerState& activePlayer =
        state.players[static_cast<std::size_t>(state.activePlayerIdx)];
    DrawActiveHandCards(activePlayer);
#endif
}

void GUIRenderer::DrawSinglePlayerStatusRow(const PlayerState& player,
                                            const int playerIndex,
                                            const int y,
                                            const int ownedPropertyCount,
                                            const bool isActive) const {
#if NIMONSPOLI_HAS_RAYLIB
    const Rectangle row = Rectangle{rightPanel_.x + 8.0f,
                                    static_cast<float>(y),
                                    rightPanel_.width - 16.0f,
                                    50.0f};

    DrawRectangleRec(
        row, isActive ? Color{226, 241, 217, 255} : Color{239, 233, 219, 255});
    DrawRectangleLinesEx(row, 1.0f, Color{126, 116, 96, 255});

    const std::string tokenKey =
        "player" + std::to_string((playerIndex % 4) + 1);
    const Rectangle iconDst =
        Rectangle{row.x + 6.0f, row.y + 12.0f, 26.0f, 26.0f};
    DrawTextureFitted(tokenKey, iconDst);

    DrawText(player.username.c_str(),
             static_cast<int>(row.x + 38.0f),
             static_cast<int>(row.y + 6.0f),
             kFontBody,
             Color{40, 40, 40, 255});

    DrawText(TextFormat("Cash: %d  Props: %d", player.balance, ownedPropertyCount),
             static_cast<int>(row.x + 38.0f),
             static_cast<int>(row.y + 28.0f),
             kFontSmall,
             Color{50, 50, 50, 255});
#endif
}

void GUIRenderer::DrawActiveHandCards(const PlayerState& activePlayer) const {
#if NIMONSPOLI_HAS_RAYLIB
    // Card aspect ratio: 243:157 (landscape) - same as deck cards
    constexpr float cardAspectRatio = 243.0f / 157.0f;

    const float cardHeight = 75.0f;
    const float cardW = cardHeight * cardAspectRatio;
    const float cardH = cardHeight;
    float x = bottomPanel_.x + 14.0f;
    const float yCard = bottomPanel_.y + 35.0f;

    for (const CardState& card : activePlayer.hand) {
        const Rectangle dst = Rectangle{x, yCard, cardW, cardH};
        DrawSingleHandCard(card, dst);

        x += cardW + 10.0f;
        if (x + cardW > bottomPanel_.x + bottomPanel_.width - 10.0f) {
            break;
        }
    }
#endif
}

void GUIRenderer::DrawSingleHandCard(const CardState& card,
                                     const Rectangle& dst) const {
#if NIMONSPOLI_HAS_RAYLIB
    const std::string key = GetHandCardTextureKey(card);
    if (HasTexture(key)) {
        DrawTextureFitted(key, dst);
    } else {
        DrawRectangleRec(dst, Color{232, 223, 206, 255});
        DrawRectangleLinesEx(dst, 1.0f, Color{115, 102, 82, 255});
        DrawText(card.type.c_str(),
                 static_cast<int>(dst.x + 6.0f),
                 static_cast<int>(dst.y + 6.0f),
                 11,
                 Color{40, 40, 40, 255});
    }
#endif
}

Color GUIRenderer::GetColorGroupForTile(const int index, const PropertyState* property) const {
    return GuiVisualModule::GetColorGroupForTile(index, property);
}

Color GUIRenderer::GetTileBodyColor(int index) const {
    (void) index;
    return Color{252, 250, 245, 255};
}

bool GUIRenderer::IsCornerTile(int index) const {
    return IsCornerTileLocal(index);
}

bool GUIRenderer::IsStreetTile(int index, const PropertyState* property) const {
    if (IsCornerTile(index))
        return false;

    if (property != nullptr) {
        std::string typeLower = ToLower(property->type);
        if (typeLower.find("street") != std::string::npos ||
            typeLower.find("property") != std::string::npos ||
            typeLower.find("lahan") != std::string::npos) {
            return true;
        }
        // Railroad and utility tiles are NOT streets
        if (typeLower.find("railroad") != std::string::npos ||
            typeLower.find("utility") != std::string::npos) {
            return false;
        }
    }

    // If there's a property with a color group, it's a street
    if (property != nullptr && !property->colorGroup.empty()) {
        return true;
    }

    // Default: non-corner tiles with no property data are treated as non-streets
    // so they get a plain rendering with possible texture/icon
    return false;
}

const PropertyState*
GUIRenderer::FindPropertyStateByCode(const GameState& state,
                                     const std::string& code) const {
    const std::string target = ToLower(code);
    for (const PropertyState& property : state.properties) {
        if (ToLower(property.code) == target) {
            return &property;
        }
    }
    return nullptr;
}

std::string
GUIRenderer::GetTileTextureKey(int tileIndex,
                               const PropertyState* propertyState) const {
    const std::string& code = tileCodesByIndex_[tileIndex];
    const std::string* propertyType =
        (propertyState != nullptr) ? &propertyState->type : nullptr;
    return GuiVisualModule::ResolveTileTextureKey(
        code, propertyType, tileTextureByCode_);
}

std::string GUIRenderer::GetHandCardTextureKey(const CardState& card) const {
    return GuiVisualModule::ResolveHandCardTextureKey(card.type);
}

void GUIRenderer::DrawTextureFitted(const std::string& key,
                                    const Rectangle& dst,
                                    const Color tint) const {
#if NIMONSPOLI_HAS_RAYLIB
    if (!HasTexture(key))
        return;

    const Texture2D tex = GetTextureOrEmpty(key);
    const Rectangle src = Rectangle{0.0f,
                                    0.0f,
                                    static_cast<float>(tex.width),
                                    static_cast<float>(tex.height)};
    DrawTexturePro(tex, src, dst, Vector2{0.0f, 0.0f}, 0.0f, tint);
#endif
}

bool GUIRenderer::HasTexture(const std::string& key) const {
    return GuiAssetModule::Has(textures_, key);
}

Texture2D GUIRenderer::GetTextureOrEmpty(const std::string& key) const {
    return GuiAssetModule::GetOrEmpty(textures_, key);
}

std::string GUIRenderer::ToLower(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
    return value;
}

bool GUIRenderer::ContainsInsensitive(const std::string& value,
                                      const std::string& needle) {
    return ToLower(value).find(ToLower(needle)) != std::string::npos;
}

bool GUIRenderer::DrawButton(const std::string& text, float x, float y, float width, float height,
                             Color bgColor, Color textColor, Color hoverColor) const {
#if NIMONSPOLI_HAS_RAYLIB
    Rectangle bounds = {x, y, width, height};
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, bounds);
    bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    Color currentBg = isHovered ? hoverColor : bgColor;
    
    // Draw button background with rounded corners
    DrawRectangleRec(bounds, currentBg);
    DrawRectangleLinesEx(bounds, 2.0f, Color{80, 80, 80, 255});
    
    // Draw text centered
    int fontSize = static_cast<int>(height * 0.4f);
    int textWidth = MeasureText(text.c_str(), fontSize);
    float textX = x + (width - textWidth) * 0.5f;
    float textY = y + (height - fontSize) * 0.5f;
    DrawText(text.c_str(), static_cast<int>(textX), static_cast<int>(textY), fontSize, textColor);
    
    return isClicked;
#else
    (void)text;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)bgColor;
    (void)textColor;
    (void)hoverColor;
    return false;
#endif
}

void GUIRenderer::DrawGameControls(float x, float y, bool canRoll, bool canEndTurn, bool isJailed) const {
#if NIMONSPOLI_HAS_RAYLIB
    float buttonWidth = 140.0f;
    float buttonHeight = 36.0f;
    float spacing = 6.0f;
    
    Color rollColor = canRoll ? Color{76, 175, 80, 255} : Color{150, 150, 150, 255};
    Color endTurnColor = canEndTurn ? Color{33, 150, 243, 255} : Color{150, 150, 150, 255};
    
    // Draw button labels with keyboard shortcuts
    DrawText("Controls:", static_cast<int>(x), static_cast<int>(y - 28), 18, DARKGRAY);
    
    // Roll Dice button
    DrawButton("Roll Dice [R]", x, y, buttonWidth, buttonHeight,
               rollColor, WHITE, Color{100, 200, 100, 255});
    
    // End Turn button
    DrawButton("End Turn [E]", x + buttonWidth + spacing, y, buttonWidth, buttonHeight,
               endTurnColor, WHITE, Color{100, 180, 255, 255});
    
    // Profile button
    DrawButton("Profile [P]", x, y + buttonHeight + spacing, buttonWidth, buttonHeight,
               Color{255, 152, 0, 255}, WHITE, Color{255, 180, 80, 255});
    
    // Build button
    DrawButton("Build [B]", x + buttonWidth + spacing, y + buttonHeight + spacing, buttonWidth, buttonHeight,
               Color{156, 39, 176, 255}, WHITE, Color{200, 100, 220, 255});
    
    // Mortgage button
    DrawButton("Mortgage [M]", x, y + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{244, 67, 54, 255}, WHITE, Color{255, 120, 100, 255});
    
    // Dismortgage button
    DrawButton("Dismortg [D]", x + buttonWidth + spacing, y + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{121, 85, 72, 255}, WHITE, Color{160, 120, 100, 255});
    
    // Use Skill button
    DrawButton("Use Skill [U]", x, y + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{0, 150, 136, 255}, WHITE, Color{80, 200, 180, 255});
    
    // Save button
    DrawButton("Save [S]", x + buttonWidth + spacing, y + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{96, 125, 139, 255}, WHITE, Color{140, 160, 170, 255});
    
    // Help button
    DrawButton("Help [H]", x, y + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{63, 81, 181, 255}, WHITE, Color{100, 130, 220, 255});
    
    // Exit button
    DrawButton("Exit [ESC]", x + buttonWidth + spacing, y + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight,
               Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255});
    
    // Pay Jail Fee button (only visible when jailed)
    if (isJailed) {
        DrawButton("Pay Jail [J]", x, y + 5 * (buttonHeight + spacing), buttonWidth, buttonHeight,
                   Color{255, 87, 34, 255}, WHITE, Color{255, 130, 100, 255});
    }
#endif
}

bool GUIRenderer::IsButtonClicked(float x, float y, float width, float height) const {
#if NIMONSPOLI_HAS_RAYLIB
    Rectangle bounds = {x, y, width, height};
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
#else
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    return false;
#endif
}