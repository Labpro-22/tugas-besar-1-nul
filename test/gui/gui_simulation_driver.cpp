#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "gui/GuiRenderer.hpp"

#if NIMONSPOLI_HAS_RAYLIB
#include <raylib.h>
#endif

static GameState BuildDummyState() {
    GameState state;
    state.currentTurn = 12;
    state.maxTurn = 150;
    state.activePlayerIdx = 0;

    PlayerState p1;
    p1.username = "Andi";
    p1.balance = 1280;
    p1.positionCode = "T01";

    PlayerState p2;
    p2.username = "Budi";
    p2.balance = 950;
    p2.positionCode = "T11";

    CardState card1;
    card1.type = "MoveCard";
    p1.hand.push_back(card1);

    CardState card2;
    card2.type = "ShieldCard";
    p1.hand.push_back(card2);

    state.players.push_back(p1);
    state.players.push_back(p2);

    PropertyState propA;
    propA.code = "T03";
    propA.type = "street";
    propA.status = "owned";
    propA.buildingCount = 2;
    propA.owner = "Andi";

    PropertyState propB;
    propB.code = "T08";
    propB.type = "street";
    propB.status = "owned";
    propB.isHotel = true;
    propB.owner = "Budi";

    PropertyState propC;
    propC.code = "T15";
    propC.type = "street";
    propC.status = "mortgage";
    propC.owner = "Andi";

    PropertyState propD;
    propD.code = "T22";
    propD.type = "street";
    propD.festivalDur = 2;
    propD.festivalMult = 2;
    propD.owner = "Budi";

    state.properties.push_back(propA);
    state.properties.push_back(propB);
    state.properties.push_back(propC);
    state.properties.push_back(propD);

    LogEntry log1;
    log1.turn = 12;
    log1.username = "Andi";
    log1.actionType = "BUY";
    log1.detail = "Membeli properti T03";
    state.log.push_back(log1);

    LogEntry log2;
    log2.turn = 12;
    log2.username = "Budi";
    log2.actionType = "BUILD";
    log2.detail = "Membangun hotel di T08";
    state.log.push_back(log2);

    return state;
}

static std::vector<std::string> BuildDefaultTileOrder() {
    std::vector<std::string> order;
    order.reserve(GUIRenderer::kBoardTileCount);
    for (int i = 1; i <= GUIRenderer::kBoardTileCount; ++i) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "T%02d", i);
        order.emplace_back(buf);
    }
    return order;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    GUIRenderer renderer;

    GUIRenderer::SetupConfig config;
    config.window.title = "Nimonspoli GUI - Fullscreen Square Tiles";
    config.window.width = 1920;
    config.window.height = 1080;
    config.window.resizable = false;
    config.assetRoot = "assets";
    config.boardVisual.tileCodesInOrder = BuildDefaultTileOrder();
    config.boardVisual.tileTextureByCode =
        GUIRenderer::BuildDefaultTileVisualMap();

    const bool assetsLoaded = renderer.Initialize(config);
    renderer.SetDiceValues(3, 5);

    GameState state = BuildDummyState();

    const GUIRenderer::BoardDisplaySnapshot snapshot =
        renderer.InspectBoardDisplay();

    std::cout << "GUI simulation initialized: "
              << (renderer.IsInitialized() ? "yes" : "no") << '\n';
    std::cout << "Assets fully loaded: " << (assetsLoaded ? "yes" : "no")
              << '\n';
    std::cout << "Board bounds: x=" << snapshot.boardBounds.x
              << ", y=" << snapshot.boardBounds.y
              << ", w=" << snapshot.boardBounds.width
              << ", h=" << snapshot.boardBounds.height << '\n';
    std::cout << "Tile size: " << snapshot.tileSize << '\n';

#if NIMONSPOLI_HAS_RAYLIB
    // Enable fullscreen
    ToggleFullscreen();
    
    int frame = 0;
    int nextAutoRollFrame = 180;  // First auto roll at 3 seconds
    bool isFullscreen = true;

    while (!WindowShouldClose()) {
        // Handle input
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
            isFullscreen = !isFullscreen;
        }
        
        if (IsKeyPressed(KEY_SPACE)) {
            // Manual dice roll
            int d1 = (std::rand() % 6) + 1;
            int d2 = (std::rand() % 6) + 1;
            renderer.StartDiceRoll(d1, d2);
            std::cout << "Manual dice roll: " << d1 << " + " << d2 << " = " << (d1+d2) << '\n';
        }
        
        if (IsKeyPressed(KEY_R)) {
            // Reset dice
            renderer.SetDiceValues(1, 1);
        }
        
        // Auto-trigger dice roll every 6 seconds
        if (frame == nextAutoRollFrame) {
            int d1 = (std::rand() % 6) + 1;
            int d2 = (std::rand() % 6) + 1;
            renderer.StartDiceRoll(d1, d2);
            std::cout << "Auto dice roll: " << d1 << " + " << d2 << " = " << (d1+d2) << '\n';
            nextAutoRollFrame = frame + 360;  // Next roll in 6 seconds
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderer.DrawBoard(state);

        // Instructions overlay - positioned at bottom right of screen
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        const char* helpText = "[SPACE] Roll  |  [F11] Fullscreen  |  [R] Reset  |  [ESC] Exit";
        int textWidth = MeasureText(helpText, 14);
        DrawText(helpText, screenW - textWidth - 20, screenH - 30, 14, Color{100, 100, 100, 180});
        
        EndDrawing();
        ++frame;
    }

    std::cout << "Visual GUI loop executed for " << frame << " frames." << '\n';
#else
    renderer.DrawBoard(state);
    std::cout << "Running in headless mode (no Raylib)." << '\n';
#endif

    renderer.Shutdown();
    std::cout << "Simulation completed." << '\n';
    return 0;
}