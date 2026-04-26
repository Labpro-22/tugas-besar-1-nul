#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "states/GameState.hpp"

#if __has_include(<raylib.h>)
#include <raylib.h>
#include <rlgl.h>
#define NIMONSPOLI_HAS_RAYLIB 1
#else
#define NIMONSPOLI_HAS_RAYLIB 0

struct Color {
    unsigned char r, g, b, a;
};
struct Rectangle {
    float x, y, width, height;
};
struct Vector2 {
    float x, y;
};
struct Texture2D {
    unsigned int id = 0;
    int width = 0, height = 0, mipmaps = 1, format = 0;
};
#endif

class GUIRenderer {
  public:
    using AssetMap = std::map<std::string, Texture2D>;

    struct WindowConfig {
        std::string title = "Nimonspoli";
        int width = 1280, height = 720;
        bool resizable = true;
    };

    struct BoardVisualConfig {
        std::vector<std::string> tileCodesInOrder;
        std::unordered_map<std::string, std::string> tileTextureByCode;
    };

    struct SetupConfig {
        WindowConfig window;
        std::string assetRoot = "assets";
        BoardVisualConfig boardVisual;
    };

    struct Square {
        float x = 0.0F, y = 0.0F, size = 0.0F;
        int side = 0;
        Rectangle asRectangle() const { return Rectangle{x, y, size, size}; }
    };

    struct BoardDisplaySnapshot {
        Rectangle boardBounds{}, centerArea{}, rightPanel{}, bottomPanel{};
        float tileSize = 0.0F;
        std::vector<Square> tileSquares;
    };

    struct DiceAnimationState {
        bool isRolling = false;
        int currentFrame = 0;
        int displayDie1 = 1;
        int displayDie2 = 1;
        int finalDie1 = 1;
        int finalDie2 = 1;
        static constexpr int kRollDuration = 60;
        static constexpr int kFrameInterval = 5;
    };

    struct TokenHopAnimation {
        bool isActive = false;
        int playerIndex = -1;
        int fromTileIndex = -1;
        int currentTileIndex = -1;
        int toTileIndex = -1;
        float hopProgress = 0.0f;
        static constexpr float kHopSpeed = 4.0f;
        static constexpr float kHopHeight = 18.0f;
    };

    static constexpr int kWindowWidthDefault = 1280;
    static constexpr int kWindowHeightDefault = 720;
    static constexpr int kBoardTileCount = 40;

    GUIRenderer();
    ~GUIRenderer();

    bool Initialize();
    bool Initialize(const SetupConfig& config);
    void Shutdown();
    bool IsInitialized() const;

    void ConfigureWindow(const std::string& title = "Nimonspoli",
                         int width = kWindowWidthDefault,
                         int height = kWindowHeightDefault,
                         bool resizable = true);

    bool LoadAssets(const std::string& assetRoot = "assets");
    bool LoadSingleAsset(const std::string& assetRoot,
                         const std::string& key,
                         const std::string& fileName);
    void UnloadAssets();
    bool IsAssetReady(const std::string& key) const;

    void SetTileOrder(const std::vector<std::string>& tileCodesInOrder);
    void SetTileVisualMap(
        const std::unordered_map<std::string, std::string>& tileTextureByCode);

    void StartDiceRoll(int finalDie1, int finalDie2);
    void SetDiceValues(int die1, int die2);
    void UpdateDiceAnimation();

    void StartTokenHop(int playerIndex, int fromTile, int toTile);
    void UpdateTokenAnimation();
    bool IsTokenAnimating() const;
    void ResetTokenAnimation();

    Square GetTileSquare(int index) const;
    int ResolveTileIndexFromCode(const std::string& code) const;
    BoardDisplaySnapshot InspectBoardDisplay() const;

    void DrawBoard(const GameState& state);

    static std::unordered_map<std::string, std::string>
    BuildDefaultTileVisualMap();

    // Button helper structure
    struct Button {
        Rectangle bounds;
        std::string text;
        Color bgColor;
        Color textColor;
        bool hovered;
        bool clicked;
    };

    // Draw a button and return true if clicked
    bool DrawButton(const std::string& text, float x, float y, float width, float height,
                    Color bgColor, Color textColor, Color hoverColor) const;

    // Draw game control buttons
    void DrawGameControls(float x, float y, bool canRoll, bool canEndTurn, bool isJailed = false) const;

    // Check if a button was clicked (to be called after drawing)
    bool IsButtonClicked(float x, float y, float width, float height) const;

  private:
    static constexpr int kBoardMargin = 24;
    static constexpr int kOuterPadding = 16;
    static constexpr int kRightPanelWidth = 300;
    static constexpr int kBottomPanelHeight = 145;
    static constexpr float kTokenScale = 0.55F;
    static constexpr float kOverlayAlphaFactor = 0.72F;
    static constexpr float kTileHeaderRatio = 0.28F;

    Rectangle boardBounds_{}, centerArea_{}, rightPanel_{}, bottomPanel_{};
    float tileSize_ = 0.0F;
    int screenWidth_ = kWindowWidthDefault;
    int screenHeight_ = kWindowHeightDefault;

    AssetMap textures_;
    std::vector<std::string> tileCodesByIndex_;
    std::unordered_map<std::string, int> tileIndexByCode_;
    std::unordered_map<std::string, std::string> tileTextureByCode_;

    int die1_ = 1, die2_ = 1;
    DiceAnimationState diceAnim_;
    TokenHopAnimation tokenHopAnim_;
    bool assetsLoaded_ = false;
    bool initialized_ = false;

    void SeedDefaultTileOrder();
    void RecalculateLayout();

    void DrawPanelsBackdrop() const;
    void DrawBaseTiles(const GameState& state) const;
    void DrawTileOverlays(const GameState& state) const;
    void DrawBuildings(const GameState& state) const;
    void DrawTokens(const GameState& state) const;
    void DrawCenterUI(const GameState& state) const;
    void DrawPlayerPanels(const GameState& state) const;

    void DrawTileBase(int index, const GameState& state) const;
    void DrawRotatedTileContent(int side,
                                const Rectangle& tileRect,
                                const Color& headerColor,
                                const std::string& code,
                                const PropertyState* property,
                                const std::string& textureKey) const;

    void DrawTileOverlayForProperty(const PropertyState& property) const;
    void DrawBuildingForProperty(const PropertyState& property) const;

    void DrawCenterLogo() const;
    void DrawDeckStacks() const;
    void DrawDiceOutsideBoard();
    void DrawTransactionLog(const GameState& state) const;

    void DrawSinglePlayerStatusRow(const PlayerState& player,
                                   int playerIndex,
                                   int y,
                                   int ownedPropertyCount,
                                   bool isActive) const;
    void DrawActiveHandCards(const PlayerState& activePlayer) const;
    void DrawSingleHandCard(const CardState& card, const Rectangle& dst) const;

    Color GetColorGroupForTile(int index, const PropertyState* property) const;
    Color GetTileBodyColor(int index) const;
    bool IsCornerTile(int index) const;
    bool IsStreetTile(int index, const PropertyState* property) const;

    const PropertyState* FindPropertyStateByCode(const GameState& state,
                                                 const std::string& code) const;
    std::string GetTileTextureKey(int tileIndex,
                                  const PropertyState* propertyState) const;
    std::string GetHandCardTextureKey(const CardState& card) const;

    void DrawTextureFitted(const std::string& key,
                           const Rectangle& dst,
                           Color tint = Color{255, 255, 255, 255}) const;

    bool HasTexture(const std::string& key) const;
    Texture2D GetTextureOrEmpty(const std::string& key) const;

    static std::string ToLower(std::string value);
    static bool ContainsInsensitive(const std::string& value,
                                    const std::string& needle);
};