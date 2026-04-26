#include "core/GameEngine.hpp"

#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/ConfigLoader.hpp"
#include "core/SaveLoadManager.hpp"
#include "config/ConfigValidator.hpp"
#include "tile/ActionTile.hpp"
#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"

#include "card/MoveCard.hpp"
#include "card/DiscountCard.hpp"
#include "card/ShieldCard.hpp"
#include "card/TeleportCard.hpp"
#include "card/LassoCard.hpp"
#include "card/DemolitionCard.hpp"

#include "player/Player.hpp"
#include "player/BotPlayer.hpp"
#include "exception/CommandException.hpp"
#include "card/ChanceCard.hpp"
#include "card/CommunityChestCard.hpp"
#include "core/TurnContext.hpp"
#include "core/TurnManager.hpp"
#include "card/SkillCard.hpp"
#include "card/DemolitionCard.hpp"
#include "card/DiscountCard.hpp"
#include "card/LassoCard.hpp"
#include "card/MoveCard.hpp"
#include "card/ShieldCard.hpp"
#include "card/TeleportCard.hpp"
#include "states/GameState.hpp"
#include "states/PlayerState.hpp"
#include "states/CardState.hpp"
#include "board/Board.hpp"
#include "tile/Tile.hpp"

#include <iomanip>
#include <limits>
#include <fstream>
#include <cstring>
#include <map>
#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>

GameEngine::GameEngine(int size)
    : board(Board{size})
        , turnmgr(TurnManager{})
        , chanceDeck(CardDeck<ChanceCard>{})
        , communityDeck(CardDeck<CommunityChestCard>{})
        , skillDeck(CardDeck<SkillCard>{})
        , status(GameStatus::NOT_STARTED)
        , activeConfig(Config{})
        , players{std::vector<Player*>{}} {
    try {
        ConfigLoader loader;
        activeConfig = loader.loadAll("config");
        ConfigValidator::validate(activeConfig);
    } catch (const std::exception& exc) {
        std::cout << "[WARN] Failed to load config files, using built-in defaults: "
                  << exc.what() << "\n";
    }
    initializeCardDecks();
}

GameEngine::~GameEngine() {
    clearPlayers();
}

void GameEngine::clearPlayers() {
    for (Player* player : players) {
        delete player;
    }
    players.clear();
}

void GameEngine::setPanelManager(GuiPanels::PanelManager* pm) {
    panelManager = pm;
}

GuiPanels::PanelManager* GameEngine::getPanelManager() const {
    return panelManager;
}

void GameEngine::run() {
    this->printBanner();
    this->startMenu();

    Dice gameDice;
    Command cmd;
    bool goNext;

    while (!turnmgr.isGameOver()) {
        Player* currentPlayer = turnmgr.getCurrentPlayer();
        if (currentPlayer == nullptr) {
            std::cout << "[WARN] No active player found. Stopping game loop.\n";
            break;
        }

        TurnContext ctx(*currentPlayer, gameDice, board, *this);
        std::cout << "══════ Turn " << (turnmgr.getCurrentTurn() + 1) << ": [" << currentPlayer->getUsername() << "] ══════\n";

        // Check if current player is a bot
        if (currentPlayer->isBot()) {
            std::cout << "[BOT] " << currentPlayer->getUsername() << " sedang berpikir...\n";
            goNext = executeBotTurn(ctx);
        } else {
            // Human player - wait for command input
            goNext = false;
            try {
                cmd.promptInput();
                goNext = cmd.dispatch(ctx);
            } catch (const CommandException& exc) {
                std::cout << exc.what() << "\n";
            } catch (const std::exception& exc) {
                std::cout << exc.what() << "\n";
            }
        }

        if (goNext) {
            turnmgr.nextTurn(ctx);
        }
    }

    std::cout << "\n=== Permainan Selesai! ===\n";
    const std::vector<Player*> winners = turnmgr.determineWinner();
    if (winners.empty()) {
        std::cout << "Tidak ada pemenang yang valid.\n";
    } else {
        std::cout << "Pemenang:\n";
        for (Player* winner : winners) {
            if (winner == nullptr) {
                continue;
            }
            std::cout << "- " << winner->getUsername()
                      << " | Balance: M" << winner->getBalance()
                      << " | Properti: " << winner->getPropertiesAmount()
                      << " | Kartu: " << winner->getHandsAmount() << "\n";
        }
    }
}

#if NIMONSPOLI_HAS_RAYLIB
static void DrawCenteredText(const char* text, int y, int fontSize, Color color) {
    int sw = GetScreenWidth();
    int textW = MeasureText(text, fontSize);
    DrawText(text, (sw - textW) / 2, y, fontSize, color);
}

static void DrawWrappedText(const std::string& text, int x, int y, int maxWidth, int fontSize, Color color, int lineHeight) {
    std::string currentLine;
    std::string word;
    int currentY = y;

    for (size_t i = 0; i <= text.size(); ++i) {
        char ch = (i < text.size()) ? text[i] : ' ';
        if (ch == ' ' || ch == '\n' || i == text.size()) {
            if (!word.empty()) {
                int lineW = MeasureText((currentLine + " " + word).c_str(), fontSize);
                if (!currentLine.empty() && lineW > maxWidth) {
                    DrawText(currentLine.c_str(), x, currentY, fontSize, color);
                    currentY += lineHeight;
                    currentLine = word;
                } else {
                    if (!currentLine.empty()) currentLine += " ";
                    currentLine += word;
                }
                word.clear();
            }
            if (ch == '\n') {
                if (!currentLine.empty()) {
                    DrawText(currentLine.c_str(), x, currentY, fontSize, color);
                    currentY += lineHeight;
                    currentLine.clear();
                }
            }
        } else {
            word += ch;
        }
    }
    if (!currentLine.empty()) {
        DrawText(currentLine.c_str(), x, currentY, fontSize, color);
    }
}

static void RenderSplashScreen(GUIRenderer& /*renderer*/, const char* message) {
    BeginDrawing();
    ClearBackground(Color{25, 25, 35, 255});

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Draw title
    const char* title = "NIMONSPOLI";
    int titleSize = 64;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (sw - titleW) / 2, sh / 2 - 80, titleSize, Color{255, 200, 50, 255});

    // Draw subtitle
    const char* subtitle = "Board Game";
    int subSize = 28;
    int subW = MeasureText(subtitle, subSize);
    DrawText(subtitle, (sw - subW) / 2, sh / 2 - 10, subSize, LIGHTGRAY);

    // Draw message
    int msgSize = 20;
    int msgW = MeasureText(message, msgSize);
    DrawText(message, (sw - msgW) / 2, sh / 2 + 60, msgSize, Color{100, 255, 150, 255});

    // Draw loading dots animation
    static int frame = 0;
    frame++;
    std::string dots = "";
    for (int i = 0; i < (frame / 30) % 4; ++i) dots += ".";
    DrawText(dots.c_str(), (sw - msgW) / 2 + msgW + 5, sh / 2 + 60, msgSize, Color{100, 255, 150, 255});

    // Draw hint
    const char* hint = "Check the terminal/console for setup prompts";
    int hintSize = 16;
    int hintW = MeasureText(hint, hintSize);
    DrawText(hint, (sw - hintW) / 2, sh / 2 + 120, hintSize, GRAY);

    EndDrawing();
}

static std::map<std::string, std::string>& GetPrevPositions() {
    static std::map<std::string, std::string> prevPositions;
    return prevPositions;
}

static void RenderGameFrame(GUIRenderer& renderer, GameEngine& engine,
                            TurnManager& turnmgr, Board& board,
                            Dice& gameDice, int& animatingDiceFrames,
                            bool showHelpOverlay = false,
                            GuiPanels::PanelManager* panelMgr = nullptr,
                            TurnContext* persistentCtx = nullptr) {
    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer == nullptr) return;

    TurnContext localCtx(*currentPlayer, gameDice, board, engine);
    TurnContext& ctx = persistentCtx ? *persistentCtx : localCtx;
    GameState state = engine.buildGameState();

    // Detect position changes and start hop animation
    auto& prevPositions = GetPrevPositions();
    for (std::size_t i = 0; i < state.players.size(); ++i) {
        const PlayerState& ps = state.players[i];
        auto it = prevPositions.find(ps.username);
        if (it != prevPositions.end() && it->second != ps.positionCode) {
            int fromIdx = renderer.ResolveTileIndexFromCode(it->second);
            int toIdx = renderer.ResolveTileIndexFromCode(ps.positionCode);
            if (fromIdx >= 1 && toIdx >= 1 && !renderer.IsTokenAnimating()) {
                renderer.StartTokenHop(static_cast<int>(i), fromIdx, toIdx);
            }
        }
        prevPositions[ps.username] = ps.positionCode;
    }

    // Update dice animation
    renderer.UpdateDiceAnimation();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw the game board
    renderer.DrawBoard(state);

    // Get board bounds for positioning UI elements
    auto snapshot = renderer.InspectBoardDisplay();

    // Button dimensions
    float controlsX = snapshot.rightPanel.x + 15;
    float controlsY = snapshot.rightPanel.y + snapshot.rightPanel.height - 280;

    // Draw turn info panel
    DrawRectangle(10, 10, 380, 110, Color{30, 30, 40, 230});
    DrawRectangleLinesEx(Rectangle{10.0f, 10.0f, 380.0f, 110.0f}, 2.0f, Color{100, 100, 120, 255});
    DrawText(TextFormat("TURN %d / %d", turnmgr.getCurrentTurn() + 1,
                        turnmgr.getMaxTurn() > 0 ? turnmgr.getMaxTurn() : 999),
             20, 18, 24, Color{255, 220, 100, 255});
    DrawText("Current Player:", 20, 48, 16, LIGHTGRAY);
    DrawText(TextFormat("%s", currentPlayer->getUsername().c_str()),
             180, 44, 26,
             currentPlayer->isBot() ? Color{255, 180, 80, 255} : Color{100, 255, 150, 255});
    DrawText(currentPlayer->isBot() ? "[BOT PLAYER]" : "[HUMAN PLAYER]",
             20, 78, 16, GRAY);

    // Draw balance info
    DrawText(TextFormat("Balance: M%d", currentPlayer->getBalance()),
             180, 78, 16, Color{255, 220, 100, 255});

    // Draw control buttons
    bool canRoll = !ctx.hasRolled &&
                   (!currentPlayer->isInJail() || (ctx.startedTurnInJail && !ctx.hasTakenJailAction));
    bool canEndTurn = ctx.canEndTurn();
    renderer.DrawGameControls(controlsX, controlsY, canRoll, canEndTurn, currentPlayer->isInJail());

    // Draw dice animation status
    if (animatingDiceFrames > 0) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        DrawRectangle(sw / 2 - 120, sh / 2 - 25, 240, 50, Color{0, 0, 0, 200});
        DrawText("Rolling Dice...", sw / 2 - 90, sh / 2 - 18, 24, WHITE);
    }

    // Draw in-game help overlay if requested
    if (showHelpOverlay) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Color{0, 0, 0, 180});
        DrawRectangle(sw / 2 - 300, 60, 600, sh - 120, Color{30, 30, 42, 240});
        DrawRectangleLinesEx(Rectangle{sw / 2.0f - 300, 60.0f, 600.0f, sh - 120.0f}, 2.0f, Color{100, 100, 120, 255});
        DrawCenteredText("P A N D U A N", 80, 32, Color{255, 200, 50, 255});
        std::string helpText =
            "Nimonspoli adalah permainan papan strategi yang terinspirasi dari Monopoly. "
            "Pemain bergerak mengelilingi papan, membeli properti, membangun rumah/hotel, dan memungut sewa.\n\n"
            "TUJUAN:\nMenjadi pemain terkaya atau satu-satunya yang tersisa setelah pemain lain bangkrut.\n\n"
            "MODE BOT:\n- Anda bisa bermain melawan 1-3 bot/komputer\n- Bot akan mengambil keputusan secara otomatis\n\n"
            "PERINTAH GERAK:\n"
            "- ROLL_DICE    : Lempar dadu untuk bergerak\n"
            "- SET_DICE X Y : Atur dadu manual (contoh: SET_DICE 3 5)\n"
            "- END_TURN     : Akhiri giliran (wajib lempar dadu dulu!)\n\n"
            "PERINTAH INFO:\n"
            "- PRINT_BOARD    : Lihat papan permainan\n"
            "- PRINT_PROPERTY : Lihat properti Anda\n"
            "- PRINT_PROP_CERT [KODE] : Lihat akta properti\n"
            "- PROFILE        : Lihat profil pemain\n"
            "- PRINT_LOG      : Lihat log transaksi\n\n"
            "PERINTAH AKSI:\n"
            "- MORTGAGE  : Gadai properti ke bank\n"
            "- BUILD     : Bangun rumah/hotel di properti\n"
            "- USE_SKILL : Gunakan kartu kemampuan\n\n"
            "PERINTAH LAIN:\n"
            "- SAVE : Simpan permainan\n"
            "- HELP : Tampilkan panduan ini";
        DrawWrappedText(helpText, sw / 2 - 280, 120, 560, 14, LIGHTGRAY, 20);
        DrawCenteredText("Press ESC or H to close", sh - 100, 16, GRAY);
    }

    // Render active GUI panel on top of everything (only after animation finishes)
    if (panelMgr && panelMgr->isActive() && !renderer.IsTokenAnimating()) {
        panelMgr->updateAndRender();
    }

    EndDrawing();
}

static bool HandleGameInput(GUIRenderer& renderer, GameEngine& engine,
                            TurnManager& turnmgr, Board& board,
                            Dice& gameDice, int& animatingDiceFrames,
                            Command& cmd, bool& goNext, bool& showHelp,
                            TurnContext& ctx,
                            GuiPanels::PanelManager* panelMgr = nullptr) {
    auto snapshot = renderer.InspectBoardDisplay();
    float controlsX = snapshot.rightPanel.x + 15;
    float controlsY = snapshot.rightPanel.y + snapshot.rightPanel.height - 300;
    const float buttonWidth = 120.0f;
    const float buttonHeight = 32.0f;
    const float spacing = 8.0f;

    bool rollClicked = renderer.IsButtonClicked(controlsX, controlsY, buttonWidth, buttonHeight);
    bool endTurnClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY, buttonWidth, buttonHeight);
    bool profileClicked = renderer.IsButtonClicked(controlsX, controlsY + buttonHeight + spacing, buttonWidth, buttonHeight);
    bool buildClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + buttonHeight + spacing, buttonWidth, buttonHeight);
    bool mortgageClicked = renderer.IsButtonClicked(controlsX, controlsY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool dismortgageClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool useSkillClicked = renderer.IsButtonClicked(controlsX, controlsY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool saveClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool helpClicked = renderer.IsButtonClicked(controlsX, controlsY + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool exitClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool payJailClicked = renderer.IsButtonClicked(controlsX, controlsY + 5 * (buttonHeight + spacing), buttonWidth, buttonHeight);

    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer == nullptr) return true;

    // Prevent ending turn while a blocking panel is active
    if (panelMgr && panelMgr->isBlocking()) {
        return false;
    }

    if (IsKeyPressed(KEY_R) || rollClicked) {
        try {
            cmd.parse("ROLL_DICE");
            goNext = cmd.dispatch(ctx);
            if (gameDice.getDie1() > 0 && gameDice.getDie2() > 0) {
                renderer.StartDiceRoll(gameDice.getDie1(), gameDice.getDie2());
                animatingDiceFrames = 90; // ~1.5 seconds at 60fps
            }
        } catch (const std::exception& exc) {
            std::cout << exc.what() << "\n";
        }
    }
    else if (IsKeyPressed(KEY_E) || endTurnClicked) {
        try {
            cmd.parse("END_TURN");
            goNext = cmd.dispatch(ctx);
        } catch (const std::exception& exc) {
            std::cout << exc.what() << "\n";
        }
    }
    else if (IsKeyPressed(KEY_P) || profileClicked) {
        try { cmd.parse("PROFILE"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_B) || buildClicked) {
        try { cmd.parse("BUILD"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_M) || mortgageClicked) {
        try { cmd.parse("MORTGAGE"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_D) || dismortgageClicked) {
        try { cmd.parse("DISMORTGAGE"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_U) || useSkillClicked) {
        try { cmd.parse("USE_SKILL"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_S) || saveClicked) {
        try { cmd.parse("SAVE"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_J) || payJailClicked) {
        if (currentPlayer->isInJail()) {
            try { cmd.parse("PAY_JAIL_FEE"); cmd.dispatch(ctx); }
            catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
        }
    }
    else if (IsKeyPressed(KEY_H) || helpClicked) {
        showHelp = true;
    }
    else if (IsKeyPressed(KEY_L)) {
        if (panelMgr) {
            panelMgr->showLog(engine.getLogger());
        }
    }
    else if (IsKeyPressed(KEY_F12)) {
        TakeScreenshot("nimonspoli_screenshot.png");
        std::cout << "[GUI] Screenshot saved to nimonspoli_screenshot.png\n";
    }
    else if (IsKeyPressed(KEY_ESCAPE) || exitClicked) {
        return true; // Signal to exit
    }

    return false;
}
#endif

#if NIMONSPOLI_HAS_RAYLIB
enum class GUIScreen {
    MAIN_MENU,
    NEW_GAME,
    LOAD_GAME,
    HELP_SCREEN,
    GAME
};

struct GUITextInput {
    std::string text;
    bool active = false;
    float backspaceHoldTime = 0.0f;

    void Update() {
        if (!active) return;

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {
                text.push_back(static_cast<char>(key));
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (!text.empty()) text.pop_back();
            backspaceHoldTime = 0.0f;
        } else if (IsKeyDown(KEY_BACKSPACE)) {
            backspaceHoldTime += GetFrameTime();
            if (backspaceHoldTime > 0.4f) {
                static float repeatAcc = 0.0f;
                repeatAcc += GetFrameTime();
                if (repeatAcc > 0.05f) {
                    repeatAcc = 0.0f;
                    if (!text.empty()) text.pop_back();
                }
            }
        } else {
            backspaceHoldTime = 0.0f;
        }
    }

    void Draw(float x, float y, float width, float height, const char* label, const char* hint = nullptr) const {
        if (label && strlen(label) > 0) {
            DrawText(label, static_cast<int>(x), static_cast<int>(y - 22), 16, DARKGRAY);
        }
        Color bg = active ? Color{240, 248, 255, 255} : WHITE;
        DrawRectangleRec(Rectangle{x, y, width, height}, bg);
        DrawRectangleLinesEx(Rectangle{x, y, width, height}, 2.0f, active ? DARKBLUE : GRAY);

        const char* displayText = text.empty() && hint != nullptr ? hint : text.c_str();
        Color textColor = text.empty() && hint != nullptr ? GRAY : BLACK;
        DrawText(displayText, static_cast<int>(x + 10), static_cast<int>(y + height / 2 - 10), 20, textColor);

        if (active && static_cast<int>(GetTime() * 2.5f) % 2 == 0) {
            int textW = MeasureText(text.c_str(), 20);
            DrawLine(static_cast<int>(x + 10 + textW), static_cast<int>(y + 6),
                     static_cast<int>(x + 10 + textW), static_cast<int>(y + height - 6), BLACK);
        }
    }

    bool CheckClick(float x, float y, float width, float height) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            bool inside = CheckCollisionPointRec(mouse, Rectangle{x, y, width, height});
            active = inside;
            return inside;
        }
        return false;
    }
};

static bool RenderGUIButton(const char* text, float x, float y, float width, float height,
                            Color bgColor, Color textColor, Color hoverColor) {
    Rectangle bounds = {x, y, width, height};
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, bounds);
    bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color currentBg = isHovered ? hoverColor : bgColor;
    DrawRectangleRec(bounds, currentBg);
    DrawRectangleLinesEx(bounds, 2.0f, Color{60, 60, 60, 255});

    int fontSize = static_cast<int>(height * 0.4f);
    int textWidth = MeasureText(text, fontSize);
    float textX = x + (width - textWidth) * 0.5f;
    float textY = y + (height - fontSize) * 0.5f;
    DrawText(text, static_cast<int>(textX), static_cast<int>(textY), fontSize, textColor);

    return isClicked;
}

static bool RenderMainMenu(GUIScreen& currentScreen) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    BeginDrawing();
    ClearBackground(Color{25, 25, 35, 255});

    DrawCenteredText("N I M O N S P O L I", sh / 2 - 180, 64, Color{255, 200, 50, 255});
    DrawCenteredText("Board Game", sh / 2 - 110, 28, LIGHTGRAY);

    float btnWidth = 300.0f;
    float btnHeight = 55.0f;
    float btnX = (sw - btnWidth) / 2.0f;
    float startY = sh / 2 - 30;
    float spacing = 20.0f;

    bool newGame = RenderGUIButton("New Game", btnX, startY, btnWidth, btnHeight,
                                   Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255});
    bool loadGame = RenderGUIButton("Load Saved Game", btnX, startY + btnHeight + spacing, btnWidth, btnHeight,
                                    Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255});
    bool help = RenderGUIButton("Help", btnX, startY + 2 * (btnHeight + spacing), btnWidth, btnHeight,
                                Color{255, 152, 0, 255}, WHITE, Color{255, 180, 80, 255});
    bool exitGame = RenderGUIButton("Exit", btnX, startY + 3 * (btnHeight + spacing), btnWidth, btnHeight,
                                    Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255});

    EndDrawing();

    if (newGame) currentScreen = GUIScreen::NEW_GAME;
    else if (loadGame) currentScreen = GUIScreen::LOAD_GAME;
    else if (help) currentScreen = GUIScreen::HELP_SCREEN;
    else if (exitGame || WindowShouldClose()) return true;

    return false;
}

struct NewGameState {
    int step = 0;
    GUITextInput maxTurnsInput;
    GUITextInput numPlayersInput;
    GUITextInput numBotsInput;
    std::vector<GUITextInput> playerNameInputs;
    std::string errorMessage;
};

static bool RenderNewGameSetup(NewGameState& state, GameEngine& /*engine*/) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Only update the input field(s) for the current step to prevent
    // previously-active fields from swallowing keystrokes.
    if (state.step == 0) {
        state.maxTurnsInput.Update();
    } else if (state.step == 1) {
        state.numPlayersInput.Update();
    } else if (state.step == 2) {
        state.numBotsInput.Update();
    } else if (state.step == 3) {
        for (auto& input : state.playerNameInputs) {
            input.Update();
        }
    }

    BeginDrawing();
    ClearBackground(Color{30, 30, 42, 255});

    DrawCenteredText("New Game Setup", 40, 36, WHITE);

    float fieldWidth = 400.0f;
    float fieldHeight = 45.0f;
    float fieldX = (sw - fieldWidth) / 2.0f;
    float startY = 120;

    if (state.step == 0) {
        DrawCenteredText("Enter maximum turns (-1 for unlimited)", startY - 10, 20, LIGHTGRAY);
        state.maxTurnsInput.Draw(fieldX, startY + 30, fieldWidth, fieldHeight, "", "-1");

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (state.maxTurnsInput.text.empty()) {
                state.maxTurnsInput.text = "-1";
            }
            try {
                int val = std::stoi(state.maxTurnsInput.text);
                if (val < -1) {
                    state.errorMessage = "Must be -1 or greater";
                } else {
                    state.errorMessage.clear();
                    state.step = 1;
                }
            } catch (...) {
                state.errorMessage = "Invalid number";
            }
        }
        if (RenderGUIButton("Next", fieldX + fieldWidth - 100, startY + 100, 100, 40,
                            Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
            if (state.maxTurnsInput.text.empty()) {
                state.maxTurnsInput.text = "-1";
            }
            try {
                int val = std::stoi(state.maxTurnsInput.text);
                if (val < -1) {
                    state.errorMessage = "Must be -1 or greater";
                } else {
                    state.errorMessage.clear();
                    state.step = 1;
                }
            } catch (...) {
                state.errorMessage = "Invalid number";
            }
        }
    } else if (state.step == 1) {
        DrawCenteredText("Enter number of players (2-4)", startY - 10, 20, LIGHTGRAY);
        state.numPlayersInput.Draw(fieldX, startY + 30, fieldWidth, fieldHeight, "", "2");

        if (RenderGUIButton("Back", fieldX, startY + 100, 100, 40,
                            Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            state.step = 0;
        }
        auto advanceFromPlayers = [&]() {
            if (state.numPlayersInput.text.empty()) {
                state.numPlayersInput.text = "2";
            }
            try {
                int val = std::stoi(state.numPlayersInput.text);
                if (val < 2 || val > 6) {
                    state.errorMessage = "Must be between 2 and 6";
                } else {
                    state.errorMessage.clear();
                    state.step = 2;
                }
            } catch (...) {
                state.errorMessage = "Invalid number";
            }
        };
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            advanceFromPlayers();
        }
        if (RenderGUIButton("Next", fieldX + fieldWidth - 100, startY + 100, 100, 40,
                            Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
            advanceFromPlayers();
        }
    } else if (state.step == 2) {
        int numPlayers = 2;
        if (!state.numPlayersInput.text.empty()) {
            try { numPlayers = std::stoi(state.numPlayersInput.text); } catch (...) {}
        }
        std::string labelText = "Enter number of bots (0-" + std::to_string(numPlayers) + ")";
        DrawCenteredText(labelText.c_str(), startY - 10, 20, LIGHTGRAY);
        state.numBotsInput.Draw(fieldX, startY + 30, fieldWidth, fieldHeight, "", "0");

        if (RenderGUIButton("Back", fieldX, startY + 100, 100, 40,
                            Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            state.step = 1;
        }
        auto advanceFromBots = [&]() {
            if (state.numBotsInput.text.empty()) {
                state.numBotsInput.text = "0";
            }
            try {
                int val = std::stoi(state.numBotsInput.text);
                if (val < 0 || val > numPlayers) {
                    state.errorMessage = "Must be between 0 and " + std::to_string(numPlayers);
                } else {
                    int numHumans = numPlayers - val;
                    if (numHumans < 1) {
                        state.errorMessage = "At least 1 human player required";
                    } else {
                        state.errorMessage.clear();
                        state.playerNameInputs.resize(numHumans);
                        for (int i = 0; i < numHumans; ++i) {
                            if (state.playerNameInputs[i].text.empty()) {
                                state.playerNameInputs[i].text = "Player" + std::to_string(i + 1);
                            }
                        }
                        state.step = 3;
                    }
                }
            } catch (...) {
                state.errorMessage = "Invalid number";
            }
        };
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            advanceFromBots();
        }
        if (RenderGUIButton("Next", fieldX + fieldWidth - 100, startY + 100, 100, 40,
                            Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
            advanceFromBots();
        }
    } else if (state.step == 3) {
        int numHumans = static_cast<int>(state.playerNameInputs.size());
        std::string titleText = "Enter names for " + std::to_string(numHumans) + " human player(s)";
        DrawCenteredText(titleText.c_str(), startY - 10, 20, LIGHTGRAY);

        float nameFieldY = startY + 20;
        float nameFieldWidth = 350.0f;
        float nameFieldHeight = 40.0f;
        float nameFieldX = (sw - nameFieldWidth) / 2.0f;

        for (int i = 0; i < numHumans; ++i) {
            std::string label = "Player " + std::to_string(i + 1);
            int labelW = MeasureText(label.c_str(), 18);
            DrawText(label.c_str(), static_cast<int>(nameFieldX - labelW - 15), static_cast<int>(nameFieldY + 8), 18, LIGHTGRAY);
            std::string hint = "Player" + std::to_string(i + 1);
            state.playerNameInputs[i].Draw(nameFieldX, nameFieldY, nameFieldWidth, nameFieldHeight, "", hint.c_str());
            nameFieldY += 55.0f;
            if (nameFieldY > sh - 150) break;
        }

        if (RenderGUIButton("Back", nameFieldX, nameFieldY + 20, 100, 40,
                            Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
            state.step = 2;
            state.playerNameInputs.clear();
        }
        auto startGame = [&]() {
            for (int i = 0; i < numHumans; ++i) {
                if (state.playerNameInputs[i].text.empty()) {
                    state.playerNameInputs[i].text = "Player" + std::to_string(i + 1);
                }
            }
            std::vector<std::string> names;
            for (const auto& input : state.playerNameInputs) {
                names.push_back(input.text);
            }
            for (size_t i = 0; i < names.size(); ++i) {
                std::string lowerName = names[i];
                for (char& c : lowerName) c = std::tolower(c);
                for (size_t j = i + 1; j < names.size(); ++j) {
                    std::string lowerCompare = names[j];
                    for (char& c : lowerCompare) c = std::tolower(c);
                    if (lowerName == lowerCompare) {
                        state.errorMessage = "Each player must have a unique name";
                        return false;
                    }
                }
            }
            state.errorMessage.clear();
            return true;
        };
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (startGame()) return true;
        }
        if (RenderGUIButton("Start Game", nameFieldX + nameFieldWidth - 140, nameFieldY + 20, 140, 40,
                            Color{76, 175, 80, 255}, WHITE, Color{100, 200, 100, 255})) {
            if (startGame()) return true;
        }
    }

    if (!state.errorMessage.empty()) {
        int errW = MeasureText(state.errorMessage.c_str(), 18);
        DrawText(state.errorMessage.c_str(), (sw - errW) / 2, sh - 80, 18, Color{255, 100, 100, 255});
    }

    // Handle clicks to activate/deactivate input fields
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (state.step == 0) state.maxTurnsInput.CheckClick(fieldX, startY + 30, fieldWidth, fieldHeight);
        else if (state.step == 1) state.numPlayersInput.CheckClick(fieldX, startY + 30, fieldWidth, fieldHeight);
        else if (state.step == 2) state.numBotsInput.CheckClick(fieldX, startY + 30, fieldWidth, fieldHeight);
        else if (state.step == 3) {
            float nameFieldY = startY + 20;
            float nameFieldWidth = 350.0f;
            float nameFieldHeight = 40.0f;
            float nameFieldX = (sw - nameFieldWidth) / 2.0f;
            for (size_t i = 0; i < state.playerNameInputs.size(); ++i) {
                state.playerNameInputs[i].CheckClick(nameFieldX, nameFieldY, nameFieldWidth, nameFieldHeight);
                nameFieldY += 55.0f;
            }
        }
    }

    EndDrawing();
    return false;
}

struct LoadGameState {
    GUITextInput filenameInput;
    std::string errorMessage;
};

static bool RenderLoadGameScreen(LoadGameState& state, GameEngine& engine, bool& backToMenu) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    state.filenameInput.Update();

    BeginDrawing();
    ClearBackground(Color{30, 30, 42, 255});

    DrawCenteredText("Load Saved Game", 40, 36, WHITE);

    float fieldWidth = 400.0f;
    float fieldHeight = 45.0f;
    float fieldX = (sw - fieldWidth) / 2.0f;
    float fieldY = sh / 2 - 50;

    DrawCenteredText("Enter save filename", fieldY - 40, 20, LIGHTGRAY);
    state.filenameInput.Draw(fieldX, fieldY, fieldWidth, fieldHeight, "", "savegame.txt");

    if (RenderGUIButton("Back", fieldX, fieldY + 80, 100, 40,
                        Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        backToMenu = true;
    }
    if (RenderGUIButton("Load", fieldX + fieldWidth - 100, fieldY + 80, 100, 40,
                        Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
        if (state.filenameInput.text.empty()) {
            state.errorMessage = "Please enter a filename";
        } else {
            try {
                engine.getBoard().generateDefaultBoard();
                engine.loadGame(state.filenameInput.text);
                state.errorMessage.clear();
                return true;
            } catch (const std::exception& e) {
                state.errorMessage = std::string("Failed to load: ") + e.what();
            }
        }
    }

    if (!state.errorMessage.empty()) {
        int errW = MeasureText(state.errorMessage.c_str(), 16);
        DrawText(state.errorMessage.c_str(), (sw - errW) / 2, fieldY + 150, 16, Color{255, 100, 100, 255});
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state.filenameInput.CheckClick(fieldX, fieldY, fieldWidth, fieldHeight);
    }

    EndDrawing();
    return false;
}

static void RenderHelpScreen(bool& backToMenu) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    BeginDrawing();
    ClearBackground(Color{25, 25, 35, 255});

    DrawCenteredText("P A N D U A N", 30, 40, Color{255, 200, 50, 255});

    std::string helpText =
        "Nimonspoli adalah permainan papan strategi yang terinspirasi dari Monopoly. "
        "Pemain bergerak mengelilingi papan, membeli properti, membangun rumah/hotel, dan memungut sewa.\n\n"
        "TUJUAN:\n"
        "Menjadi pemain terkaya atau satu-satunya yang tersisa setelah pemain lain bangkrut.\n\n"
        "MODE BOT:\n"
        "- Anda bisa bermain melawan 1-3 bot/komputer\n"
        "- Bot akan mengambil keputusan secara otomatis\n\n"
        "PERINTAH GERAK:\n"
        "- ROLL_DICE    : Lempar dadu untuk bergerak\n"
        "- SET_DICE X Y : Atur dadu manual (contoh: SET_DICE 3 5)\n"
        "- END_TURN     : Akhiri giliran (wajib lempar dadu dulu!)\n\n"
        "PERINTAH INFO:\n"
        "- PRINT_BOARD    : Lihat papan permainan\n"
        "- PRINT_PROPERTY : Lihat properti Anda\n"
        "- PRINT_PROP_CERT [KODE] : Lihat akta properti\n"
        "- PROFILE        : Lihat profil pemain\n"
        "- PRINT_LOG      : Lihat log transaksi\n\n"
        "PERINTAH AKSI:\n"
        "- MORTGAGE  : Gadai properti ke bank\n"
        "- BUILD     : Bangun rumah/hotel di properti\n"
        "- USE_SKILL : Gunakan kartu kemampuan\n\n"
        "PERINTAH LAIN:\n"
        "- SAVE : Simpan permainan\n"
        "- HELP : Tampilkan panduan ini";

    DrawWrappedText(helpText, 60, 90, sw - 120, 16, LIGHTGRAY, 22);

    float btnWidth = 120.0f;
    float btnHeight = 40.0f;
    if (RenderGUIButton("Back", (sw - btnWidth) / 2.0f, sh - 80, btnWidth, btnHeight,
                        Color{120, 120, 120, 255}, WHITE, Color{160, 160, 160, 255})) {
        backToMenu = true;
    }

    EndDrawing();
}
#endif

void GameEngine::runGUI() {
#if NIMONSPOLI_HAS_RAYLIB
    // Initialize GUI renderer
    GUIRenderer renderer;
    GUIRenderer::SetupConfig config;
    config.window.title = "Nimonspoli - Board Game";
    config.window.width = 1400;
    config.window.height = 900;
    config.window.resizable = true;
    config.assetRoot = "assets";

    for (int i = 0; i < GUIRenderer::kBoardTileCount; ++i) {
        const Tile* tile = board.getTile(i);
        if (tile != nullptr) {
            config.boardVisual.tileCodesInOrder.push_back(tile->getCode());
        } else {
            std::ostringstream oss;
            oss << "T" << std::setw(2) << std::setfill('0') << (i + 1);
            config.boardVisual.tileCodesInOrder.push_back(oss.str());
        }
    }
    config.boardVisual.tileTextureByCode = GUIRenderer::BuildDefaultTileVisualMap();

    if (!renderer.Initialize(config)) {
        std::cerr << "[ERROR] Failed to initialize GUI renderer\n";
        std::cout << "Falling back to console mode...\n";
        run();
        return;
    }

    auto syncTileOrderFromBoard = [&renderer, this]() {
        std::vector<std::string> codes;
        for (int i = 0; i < GUIRenderer::kBoardTileCount; ++i) {
            const Tile* tile = board.getTile(i);
            if (tile != nullptr) {
                codes.push_back(tile->getCode());
            } else {
                std::ostringstream oss;
                oss << "T" << std::setw(2) << std::setfill('0') << (i + 1);
                codes.push_back(oss.str());
            }
        }
        renderer.SetTileOrder(codes);
    };

    syncTileOrderFromBoard();

    bool shouldExit = false;
    while (!shouldExit && !WindowShouldClose()) {
        GuiPanels::PanelManager panelManager;
        setPanelManager(&panelManager);

    // Show splash screen briefly
    double splashStart = GetTime();
    while (GetTime() - splashStart < 1.5 && !WindowShouldClose()) {
        RenderSplashScreen(renderer, "Starting game");
    }

    // GUI Menu System - replaces console-based menu in GUI mode
    GUIScreen currentScreen = GUIScreen::MAIN_MENU;
    NewGameState newGameState;
    LoadGameState loadGameState;

    // Menu loop
    while (!WindowShouldClose() && currentScreen != GUIScreen::GAME) {
        if (currentScreen == GUIScreen::MAIN_MENU) {
            if (RenderMainMenu(currentScreen)) {
                renderer.Shutdown();
                return; // Exit requested
            }
        }
        else if (currentScreen == GUIScreen::NEW_GAME) {
            bool ready = RenderNewGameSetup(newGameState, *this);
            if (ready) {
                // Create the game from GUI inputs
                int maxTurns = std::stoi(newGameState.maxTurnsInput.text);
                int numPlayers = std::stoi(newGameState.numPlayersInput.text);
                int numBots = std::stoi(newGameState.numBotsInput.text);
                int numHumans = numPlayers - numBots;

                if (maxTurns < -1) {
                    maxTurns = getConfiguredMaxTurn();
                }

                board.generateDefaultBoard();
                syncTileOrderFromBoard();
                GetPrevPositions().clear();
                renderer.ResetTokenAnimation();
                turnmgr = TurnManager(maxTurns);

                clearPlayers();
                for (int i = 0; i < numHumans; ++i) {
                    players.push_back(new Player(newGameState.playerNameInputs[i].text, getStartingBalance()));
                }
                for (int i = 0; i < numBots; ++i) {
                    std::string botName = "Bot" + std::to_string(i + 1);
                    players.push_back(new BotPlayer(botName, getStartingBalance()));
                }
                for (Player* player : players) {
                    if (player != nullptr) {
                        giveRandomSkillCardTo(*player);
                    }
                }
                turnmgr.setTurnOrder(getPlayers());
                currentScreen = GUIScreen::GAME;
            }
            if (IsKeyPressed(KEY_ESCAPE) && newGameState.step == 0) {
                currentScreen = GUIScreen::MAIN_MENU;
                newGameState = NewGameState{};
            }
        }
        else if (currentScreen == GUIScreen::LOAD_GAME) {
            bool backToMenu = false;
            bool loaded = RenderLoadGameScreen(loadGameState, *this, backToMenu);
            if (loaded) {
                syncTileOrderFromBoard();
                GetPrevPositions().clear();
                renderer.ResetTokenAnimation();
                currentScreen = GUIScreen::GAME;
            } else if (backToMenu) {
                currentScreen = GUIScreen::MAIN_MENU;
                loadGameState = LoadGameState{};
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentScreen = GUIScreen::MAIN_MENU;
            }
        }
        else if (currentScreen == GUIScreen::HELP_SCREEN) {
            bool backToMenu = false;
            RenderHelpScreen(backToMenu);
            if (backToMenu || IsKeyPressed(KEY_ESCAPE)) {
                currentScreen = GUIScreen::MAIN_MENU;
            }
        }
    }

    if (WindowShouldClose()) {
        break;
    }

    // Show splash again briefly before game
    splashStart = GetTime();
    while (GetTime() - splashStart < 1.0 && !WindowShouldClose()) {
        RenderSplashScreen(renderer, "Loading board");
    }

    Dice gameDice;
    Command cmd;
    bool goNext = false;
    int animatingDiceFrames = 0;
    bool showInGameHelp = false;

    // Main game loop
    while (!turnmgr.isGameOver() && !shouldExit) {
        if (WindowShouldClose()) {
            shouldExit = true;
            break;
        }

        goNext = false; // Reset turn-advance flag at start of each turn

        Player* currentPlayer = turnmgr.getCurrentPlayer();
        if (currentPlayer == nullptr) {
            std::cout << "[WARN] No active player found. Stopping game loop.\n";
            break;
        }

        // Handle bot turns automatically
        if (currentPlayer->isBot()) {
            std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1)
                      << ": " << currentPlayer->getUsername() << " (BOT) ===\n";

            TurnContext ctx(*currentPlayer, gameDice, board, *this);

            renderer.SetDiceValues(1, 1);
            renderer.StartDiceRoll(
                (std::rand() % 6) + 1,
                (std::rand() % 6) + 1
            );
            animatingDiceFrames = 90;

            double animStart = GetTime();
            while (GetTime() - animStart < 1.2 && !WindowShouldClose()) {
                GameState state = buildGameState();
                renderer.UpdateDiceAnimation();
                BeginDrawing();
                ClearBackground(RAYWHITE);
                renderer.DrawBoard(state);
                EndDrawing();
            }

            goNext = executeBotTurn(ctx);

            if (gameDice.getDie1() > 0 && gameDice.getDie2() > 0) {
                renderer.StartDiceRoll(gameDice.getDie1(), gameDice.getDie2());
                animatingDiceFrames = 90;
            }

            double postTurnStart = GetTime();
            while (GetTime() - postTurnStart < 0.5 && !WindowShouldClose()) {
                RenderGameFrame(renderer, *this, turnmgr, board, gameDice, animatingDiceFrames);
            }

            if (goNext) {
                turnmgr.nextTurn(ctx);
            }
            continue;
        }

        // Human player turn - persist TurnContext across frames
        {
            TurnContext ctx(*currentPlayer, gameDice, board, *this);
            bool humanTurnDone = false;

            while (!humanTurnDone && !WindowShouldClose()) {
                // Only print turn info once per human turn
                static Player* lastPrintedPlayer = nullptr;
                static int lastPrintedTurn = -1;
                if (currentPlayer != lastPrintedPlayer || turnmgr.getCurrentTurn() != lastPrintedTurn) {
                    std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1)
                              << ": " << currentPlayer->getUsername() << " ===\n";
                    lastPrintedPlayer = currentPlayer;
                    lastPrintedTurn = turnmgr.getCurrentTurn();
                }

                // Render frame (with optional help overlay and panels)
                RenderGameFrame(renderer, *this, turnmgr, board, gameDice, animatingDiceFrames, showInGameHelp, &panelManager, &ctx);

                // Decrement animation counter
                if (animatingDiceFrames > 0) {
                    animatingDiceFrames--;
                }

                // Toggle off in-game help overlay
                if (showInGameHelp && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_H))) {
                    showInGameHelp = false;
                    continue;
                }

                // If a blocking panel is active, skip normal input this frame
                if (panelManager.isBlocking()) {
                    continue;
                }

                // If token hop animation is active, skip input until it finishes
                if (renderer.IsTokenAnimating()) {
                    continue;
                }

                // Handle input
                bool inputExit = HandleGameInput(renderer, *this, turnmgr, board,
                                                  gameDice, animatingDiceFrames, cmd, goNext, showInGameHelp, ctx, &panelManager);
                if (inputExit) {
                    shouldExit = true;
                    humanTurnDone = true;
                    break;
                }

                if (goNext) {
                    turnmgr.nextTurn(ctx);
                    lastPrintedPlayer = nullptr;
                    lastPrintedTurn = -1;
                    humanTurnDone = true;
                    break;
                }

                // Support console fallback input with Enter key
                if (IsKeyPressed(KEY_ENTER)) {
                    std::cout << "\n> ";
                    std::string input;
                    std::getline(std::cin, input);
                    if (!input.empty()) {
                        try {
                            cmd.parse(input);
                            goNext = cmd.dispatch(ctx);
                            if (goNext) {
                                turnmgr.nextTurn(ctx);
                                lastPrintedPlayer = nullptr;
                                lastPrintedTurn = -1;
                                humanTurnDone = true;
                                break;
                            }
                        } catch (const std::exception& exc) {
                            std::cout << exc.what() << "\n";
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n=== Permainan Selesai! ===\n";
    const std::vector<Player*> winners = turnmgr.determineWinner();
    if (winners.empty()) {
        std::cout << "Tidak ada pemenang yang valid.\n";
    } else {
        std::cout << "Pemenang:\n";
        for (Player* winner : winners) {
            if (winner == nullptr) {
                continue;
            }
            std::cout << "- " << winner->getUsername()
                      << " | Balance: M" << winner->getBalance()
                      << " | Properti: " << winner->getPropertiesAmount()
                      << " | Kartu: " << winner->getHandsAmount() << "\n";
        }
    }

    bool returnToMenu = false;

    #if NIMONSPOLI_HAS_RAYLIB
    {
        bool showGameOverScreen = true;
        while (showGameOverScreen && !WindowShouldClose()) {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            BeginDrawing();
            ClearBackground(Color{25, 25, 35, 255});

            DrawCenteredText("P E R M A I N A N   S E L E S A I", sh / 2 - 120, 48, Color{255, 200, 50, 255});

            if (!winners.empty()) {
                std::string winnerText = "Pemenang: " + winners[0]->getUsername();
                int wealth = winners[0]->getWealth();
                winnerText += " (Kekayaan: M" + std::to_string(wealth) + ")";
                DrawCenteredText(winnerText.c_str(), sh / 2 - 40, 28, Color{100, 255, 150, 255});

                if (winners.size() > 1) {
                    std::string tieText = "Seri dengan: ";
                    for (size_t i = 1; i < winners.size(); ++i) {
                        if (i > 1) tieText += ", ";
                        tieText += winners[i]->getUsername();
                    }
                    DrawCenteredText(tieText.c_str(), sh / 2, 20, Color{255, 220, 100, 255});
                }
            } else {
                DrawCenteredText("Tidak ada pemenang", sh / 2 - 40, 28, LIGHTGRAY);
            }

            std::string statsText = "Giliran Total: " + std::to_string(turnmgr.getCurrentTurn() + 1);
            DrawCenteredText(statsText.c_str(), sh / 2 + 50, 18, GRAY);

            float btnWidth = 200.0f;
            float btnHeight = 45.0f;
            float btnX = (sw - btnWidth) / 2.0f;
            float btnY = sh / 2 + 100;

            if (RenderGUIButton("Main Menu", btnX, btnY, btnWidth, btnHeight,
                               Color{33, 150, 243, 255}, WHITE, Color{100, 180, 255, 255})) {
                showGameOverScreen = false;
                returnToMenu = true;
            }

            btnY += btnHeight + 12;
            if (RenderGUIButton("Keluar", btnX, btnY, btnWidth, btnHeight,
                               Color{244, 67, 54, 255}, WHITE, Color{255, 120, 100, 255})) {
                showGameOverScreen = false;
                shouldExit = true;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                showGameOverScreen = false;
                shouldExit = true;
            }

            EndDrawing();
        }
    }
    #endif

    if (returnToMenu) {
        clearPlayers();
        continue;
    }

    break;
    }

    renderer.Shutdown();
#else
    std::cout << "GUI mode not available - Raylib not found.\n";
    std::cout << "Running in console mode...\n";
    run();
#endif
}

bool GameEngine::executeBotTurn(TurnContext& ctx) {
    BotPlayer* bot = dynamic_cast<BotPlayer*>(&ctx.currentPlayer);
    if (bot == nullptr) {
        return false;
    }

    // Mark that bot has taken action (for save restriction)
    turnmgr.markActionTaken();

    // Bot rolls dice automatically
    std::cout << "[BOT] " << bot->getUsername() << " melempar dadu...\n";

    bool validRoll = ctx.dice.roll();
    if (!validRoll) {
        std::cout << "[WARN] Bot cannot roll the dice anymore this turn.\n";
        return true; // End turn
    }

    int diceTotal = ctx.dice.getTotal();
    std::cout << "[BOT] Hasil: " << ctx.dice.getDie1() << "+" << ctx.dice.getDie2() << " = " << diceTotal << "\n";

    // Log dice roll
    std::string logDetail = "Lempar: " + std::to_string(ctx.dice.getDie1()) + "+" +
                            std::to_string(ctx.dice.getDie2()) + "=" + std::to_string(diceTotal);
    logAction(bot->getUsername(), "DADU", logDetail);

    // Move bot
    int nextPos = bot->move(diceTotal, ctx);
    Tile* baseTile = board.getTile(nextPos);
    if (baseTile == nullptr) {
        std::cout << "[ERROR] Bot moved to invalid tile.\n";
        return true;
    }

    std::cout << "[BOT] " << bot->getUsername() << " mendarat di " << baseTile->getName() << "\n";

    // Log movement
    logDetail = "Mendarat di " + baseTile->getName() + " (" + baseTile->getCode() + ")";
    logAction(bot->getUsername(), "GERAK", logDetail);

    // Handle property tile
    PropertyTile* propTile = dynamic_cast<PropertyTile*>(baseTile);
    if (propTile != nullptr) {
        Property* prop = propTile->getProperty();
        if (prop != nullptr && prop->getStatus() == PropertyStatus::BANK) {
            // Bot decides whether to buy
            if (bot->decideBuyProperty(prop->getBuyPrice(), bot->getBalance())) {
                try {
                    bot->buy(prop, ctx);
                    std::cout << "[BOT] " << bot->getUsername() << " membeli " << prop->getName() << " seharga M" << prop->getBuyPrice() << "\n";
                    logAction(bot->getUsername(), "BELI", "Beli " + prop->getName() + " (" + prop->getCode() + ") seharga M" + std::to_string(prop->getBuyPrice()));
                } catch (const std::exception& e) {
                    std::cout << "[BOT] Gagal membeli: " << e.what() << "\n";
                }
            } else {
                std::cout << "[BOT] " << bot->getUsername() << " memutuskan untuk tidak membeli.\n";
            }
        }
    }
    // Trigger tile onLanded
    baseTile->onLanded(ctx);

    // Small delay for readability
    std::cout << "[BOT] Giliran " << bot->getUsername() << " selesai.\n";

    return true; // End bot's turn
}

void GameEngine::loadGameFromSave() {
    clearPlayers();

    std::cout << "[INFO] Querying Save File...\n";
    std::cout << "Input game saved file\n";
    std::cout << "> ";
    std::string filename;
    std::getline(std::cin, filename);

    if (filename.empty()) {
        std::cout << "Filename can't be empty!\n\n";
        return;
    }

    try {
        loadGame(filename);
    } catch (CommandException e) {
        std::cout << e.what() << "\n";
        std::cout << "Generating default configuration...\n\n";
        // Fallback ke new game jika load gagal
        this->newGame();
    }
}

void GameEngine::newGame() {
    std::cout << "[INIT] Starting New Game\n";
    std::cout << "[INIT] Generating default board\n";
    this->board.generateDefaultBoard();

    int maxTurns = getConfiguredMaxTurn(); //tergantung, kalau new game masih bisa ngeload ya brarti bener gini
    std::cout << "[INIT] Querying max turns\n";
    std::cout << "Enter max turn (-1 for unlimited turns, default from config: " << maxTurns << ")\n";
    std::cout << "Note that turns mean each player's turn.\n";
    std::cout << "> ";
    while (!(std::cin >> maxTurns)){
        std::cout << "Input invalid! Try again! Enter max turn (-1 for unlimited turns)\n";std::cin.clear();
        std::cout << "Note that turns mean each player's turn.\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    };
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (maxTurns < -1){
        maxTurns = -1; //additional validation
    }
    this->turnmgr = TurnManager(maxTurns);
    std::cout << "\n";

    int numPlayers;
    std::cout << "[INIT] Querying total game players\n";
    std::cout << "Enter total player amount (2-4)\n";
    std::cout << "> ";
    while (!(std::cin >> numPlayers) || numPlayers < 2 || numPlayers > 4) {
        std::cout << "Amount must be a valid number (2-4)! Retrying input...\n\n";
        std::cout << "Enter total player amount (2-4)\n";
        std::cout << "> ";

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "\n";
    std::cout << "[INIT] Total player amount set to " << numPlayers << "\n";
    std::cout << "\n";

    int numBots;
    std::cout << "[INIT] Querying bot players\n";
    std::cout << "Enter bot amount (0-" << numPlayers-1 << ")\n";
    std::cout << "> ";
    while (!(std::cin >> numBots)) {
        std::cout << "Amount must be a valid number (0-" << std::to_string(numPlayers-1) << ")! Retrying input...\n\n";
        std::cout << "Enter bot amount (0-" << numPlayers-1 << ")\n";
        std::cout << "> ";

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (numBots < 0) { std::cout << "Number lower than 0!\n"; numBots = 0; }
    else if (numBots > numPlayers-1) { std::cout << "Number larger than " << numPlayers-1 << "\n"; numBots = numPlayers-1; }
    else { std::cout << "[INIT] Bot amount set to " << std::to_string(numBots) << "\n"; }
    std::cout << "\n";

    int numHumans = numPlayers - numBots;
    std::cout << "[INIT] Human amount set to " << std::to_string(numHumans) << "\n";
    std::cout << "\n";

    std::cout << "[INIT] Querying human player names\n";
    for (int i = 0; i < numHumans; ++i) {
        std::string username;
        std::cout << "Enter player " << (i + 1) << "'s name\n";
        std::cout << "> ";
        while (true){
            std::getline(std::cin, username);
            if (username != ""){
                break;
            } else{ std::cout << "Username can't be empty! Retrying input...\n"; }
        }
        players.push_back(new Player(username, getStartingBalance()));
        std::cout << "[INIT] Added human player: " << username << "\n\n";
    }

    for (int i = 0; i < numBots; ++i) {
        std::string botName = "Bot" + std::to_string(i + 1);
        players.push_back(new BotPlayer(botName, getStartingBalance()));
        std::cout << "[INIT] Added bot player: " << botName << "\n";
        if (i == numBots - 1) std::cout << "\n";
    }

    std::cout << "[INIT] Distributing initial skill cards\n";
    for (Player* player : players) {
        if (player != nullptr) {
            giveRandomSkillCardTo(*player);
        }
    }
    std::cout << "\n";

    turnmgr.setTurnOrder(this->getPlayers());
}



std::vector<Player*> GameEngine::getPlayers() const {
    return players;
}

void GameEngine::displayPlayers() const {
    std::cout << "\n=== Daftar Pemain ===\n";
    for (const auto& player : players) {
        std::cout << "Pemain: " << player->getUsername()
                  << " | Posisi: " << player->getPosition()
                  << " | Kekayaan: " << player->getWealth() << "\n";
    }
    std::cout << "=====================\n\n";
}

void GameEngine::printBanner() {
    cout << "\n";
    cout
        << "  "
           "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                              "
            "║\n";
    cout << "  ║     ███╗   ██╗██╗███╗   ███╗ ██████╗ ███╗   ██╗███████╗      "
            "║\n";
    cout << "  ║     ████╗  ██║██║████╗ ████║██╔═══██╗████╗  ██║██╔════╝      "
            "║\n";
    cout << "  ║     ██╔██╗ ██║██║██╔████╔██║██║   ██║██╔██╗ ██║███████╗      "
            "║\n";
    cout << "  ║     ██║╚██╗██║██║██║╚██╔╝██║██║   ██║██║╚██╗██║╚════██║      "
            "║\n";
    cout << "  ║     ██║ ╚████║██║██║ ╚═╝ ██║╚██████╔╝██║ ╚████║███████║      "
            "║\n";
    cout << "  ║     ╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝      "
            "║\n";
    cout << "  ║                                                              "
            "║\n";
    cout << "  ║              ██████╗  ██████╗ ██╗     ███████╗               "
            "║\n";
    cout << "  ║              ██╔══██╗██╔═══██╗██║     ╚═███╔═╝               "
            "║\n";
    cout << "  ║              ██████╔╝██║   ██║██║       ███║                 "
            "║\n";
    cout << "  ║              ██╔═══╝ ██║   ██║██║       ███║                 "
            "║\n";
    cout << "  ║              ██║     ╚██████╔╝███████╗███████╗               "
            "║\n";
    cout << "  ║              ╚═╝      ╚═════╝ ╚══════╝╚══════╝               "
            "║\n";
    cout << "  ║                                                              "
            "║\n";
    cout
        << "  "
           "╚══════════════════════════════════════════════════════════════╝\n";
}

void GameEngine::startMenu() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "             WELCOME TO\n";
    std::cout << "         N I M O N S P O L I\n";
    std::cout << "========================================\n";
    std::cout << "Pick an option:\n";
    std::cout << "  1. NEW GAME       - Start a new game\n";
    std::cout << "  2. LOAD SAVED GAME - Continue from save file\n";
    std::cout << "  3. HELP           - Show the help display\n";
    std::cout << "  4. EXIT           - Exit the game\n";

    while (true) {
        std::cout << "Enter a choice (1-4)\n";
        std::cout << "> ";
        std::string choiceLine;
        std::getline(std::cin, choiceLine);

        int choice = 0;
        try {
            choice = std::stoi(choiceLine);
        } catch (const std::exception&) {
            std::cout << "Input has to be 1-4. Please retry\n\n";
            continue;
        }

        if (choice == 1) {
            std::cout << "\n";
            this->newGame();
            break;
        } else if (choice == 2) {
            std::cout << "\n";
            this->loadGameFromSave();
            break;
        } else if (choice == 3) {
            break;
        } else if (choice == 4) {
            std::cout << "\n";
            std::cout << "Thank you for playing!\n";
            exit(0);
            break;
        } else {
            std::cout << "Input has to be 1-4. Please retry\n\n";
        }
    }
}

TurnManager& GameEngine::getTurnManager() {
    return turnmgr;
}

int GameEngine::getConfiguredMaxTurn() const {
    return (activeConfig.misc.maxTurn > 0) ? activeConfig.misc.maxTurn : -1;
}

int GameEngine::getGoSalary() const {
    return (activeConfig.special.goSalary > 0) ? activeConfig.special.goSalary : 200;
}

int GameEngine::getJailFine() const {
    return (activeConfig.special.jailFine > 0) ? activeConfig.special.jailFine : 50;
}

int GameEngine::getTaxPphFlat() const {
    return (activeConfig.tax.pphFlat >= 0) ? activeConfig.tax.pphFlat : 0;
}

int GameEngine::getTaxPphPercent() const {
    if (activeConfig.tax.pphPercent < 0) {
        return 0;
    }
    if (activeConfig.tax.pphPercent > 100) {
        return 100;
    }
    return activeConfig.tax.pphPercent;
}

int GameEngine::getTaxPbmFlat() const {
    return (activeConfig.tax.pbmFlat >= 0) ? activeConfig.tax.pbmFlat : 150;
}

int GameEngine::getStartingBalance() const {
    return (activeConfig.misc.startingBalance > 0) ? activeConfig.misc.startingBalance : 1500;
}

void GameEngine::giveRandomSkillCardTo(Player& player) {
    if (player.getHandsAmount() >= 3) {
        return;
    }

    if (skillDeck.size() == 0) {
        return;
    }

    SkillCard* drawn = skillDeck.draw();
    if (drawn == nullptr) {
        return;
    }

    player.drawSCard(drawn);
}

void GameEngine::initializeCardDecks() {
    // Initialize Chance Cards (Kesempatan)
    chanceDeck.addCard(new ChanceCard("Pergi ke stasiun terdekat", ChanceType::GO_TO_NEAREST_STATION));
    chanceDeck.addCard(new ChanceCard("Mundur 3 petak", ChanceType::MOVE_BACK_3));
    chanceDeck.addCard(new ChanceCard("Masuk Penjara", ChanceType::GO_TO_JAIL));
    chanceDeck.shuffle();

    // Initialize Community Chest Cards (Dana Umum)
    communityDeck.addCard(new CommunityChestCard("Ulang Tahun! Dapat M100 dari setiap pemain", CommunityChestType::BIRTHDAY, 100));
    communityDeck.addCard(new CommunityChestCard("Biaya Dokter. Bayar M700", CommunityChestType::DOCTOR_FEE, 700));
    communityDeck.addCard(new CommunityChestCard("Kampanye Politik. Bayar M200 ke setiap pemain", CommunityChestType::POLITICAL_CAMPAIGN, 200));
    communityDeck.shuffle();

    // Initialize Skill Cards (Kartu Kemampuan Spesial)
    // Random number generators for skill card values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> moveDist(1, 6);        // MoveCard: 1-6 steps
    std::uniform_int_distribution<> discountDist(10, 50);  // DiscountCard: 10-50%
    std::uniform_int_distribution<> teleportDist(0, 39);   // TeleportCard: position 0-39

    // 4 MoveCards with random steps
    for (int i = 0; i < 4; ++i) {
        skillDeck.addCard(new MoveCard(moveDist(gen)));
    }

    // 3 DiscountCards with random discount percentage
    for (int i = 0; i < 3; ++i) {
        skillDeck.addCard(new DiscountCard(discountDist(gen)));
    }

    // 2 ShieldCards
    for (int i = 0; i < 2; ++i) {
        skillDeck.addCard(new ShieldCard());
    }

    // 2 TeleportCards with random position
    for (int i = 0; i < 2; ++i) {
        skillDeck.addCard(new TeleportCard(teleportDist(gen)));
    }

    // 2 LassoCards
    for (int i = 0; i < 2; ++i) {
        skillDeck.addCard(new LassoCard());
    }

    // 2 DemolitionCards
    for (int i = 0; i < 2; ++i) {
        skillDeck.addCard(new DemolitionCard());
    }

    skillDeck.shuffle();
    std::cout << "[INFO] Skill deck initialized with " << skillDeck.size() << " cards.\n";
}

ChanceCard* GameEngine::drawChanceCard() {
    if (chanceDeck.size() == 0) {
        chanceDeck.refillFromDiscard();
    }
    ChanceCard* card = chanceDeck.draw();
    if (card != nullptr) {
        std::cout << "[INFO] Mengambil kartu Kesempatan dari deck.\n";
    }
    return card;
}

CommunityChestCard* GameEngine::drawCommunityChestCard() {
    if (communityDeck.size() == 0) {
        communityDeck.refillFromDiscard();
    }
    CommunityChestCard* card = communityDeck.draw();
    if (card != nullptr) {
        std::cout << "[INFO] Mengambil kartu Dana Umum dari deck.\n";
    }
    return card;
}

void GameEngine::returnChanceCard(ChanceCard* card) {
    if (card != nullptr) {
        chanceDeck.addToDiscard(card);
    }
}

void GameEngine::returnCommunityChestCard(CommunityChestCard* card) {
    if (card != nullptr) {
        communityDeck.addToDiscard(card);
    }
}

TransactionLogger& GameEngine::getLogger() {
    return logger;
}

void GameEngine::logAction(const std::string& user, const std::string& action, const std::string& detail) {
    logger.log(turnmgr.getCurrentTurn(), user, action, detail);
}

void GameEngine::printLogs(int n) {
    logger.printLogs(n);
}

GameState GameEngine::buildGameState() const {
    GameState state;
    state.currentTurn = turnmgr.getCurrentTurn();
    state.maxTurn = turnmgr.getMaxTurn();
    state.activePlayerIdx = turnmgr.getActivePlayerIndex();

    // Build player states
    for (const auto& player : players) {
        PlayerState ps;
        ps.username = player->getUsername();
        ps.balance = player->getBalance();
        ps.isBot = player->isBot();

        // Get position code from board
        const Tile* tile = board.getTile(player->getPosition());
        ps.positionCode = tile != nullptr ? tile->getCode() : "GO";

        // Get status
        switch (player->getStatus()) {
            case PlayerStatus::JAILED:
                ps.status = "JAILED";
                break;
            case PlayerStatus::BANKRUPT:
                ps.status = "BANKRUPT";
                break;
            default:
                ps.status = "ACTIVE";
                break;
        }

        // Save hand cards
        for (SkillCard* card : player->getHand()) {
            CardState cs;
            cs.type = card->getTypeName();
            cs.value = card->getSkillValue();
            cs.remainingDuration = card->getRemainingDuration();
            ps.hand.push_back(cs);
        }

        state.players.push_back(ps);
    }

    // Build turn order
    for (Player* player : turnmgr.getTurnOrder()) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] == player) {
                state.turnOrder.push_back(static_cast<int>(i));
                break;
            }
        }
    }

    // Build property states from board
    for (Property* prop : board.getAllProperties()) {
        if (prop == nullptr) continue;

        PropertyState propState;
        propState.code = prop->getCode();
        propState.name = prop->getName();

        // Determine property type
        if (dynamic_cast<StreetProperty*>(prop) != nullptr) {
            propState.type = "street";
            StreetProperty* street = dynamic_cast<StreetProperty*>(prop);
            propState.colorGroup = street->getColorGroup();
            propState.buildingCount = street->getBuildingCount();
            propState.isHotel = street->hasHotel();
        } else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
            propState.type = "railroad";
        } else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
            propState.type = "utility";
        } else {
            propState.type = "property";
        }

        // Get owner
        Player* owner = prop->getOwner();
        if (owner != nullptr) {
            propState.owner = owner->getUsername();
        } else{
            propState.owner = "BANK";
        }

        // Get status
        switch (prop->getStatus()) {
            case PropertyStatus::OWNED:
                propState.status = "OWNED";
                break;
            case PropertyStatus::MORTGAGED:
                propState.status = "MORTGAGED";
                break;
            default:
                propState.status = "BANK";
                break;
        }

        // Festival info
        propState.festivalMult = prop->getFestivalMultiplier();
        propState.festivalDur = prop->getFestivalDuration();

        state.properties.push_back(propState);
    }

    // Save remaining skill deck state using the same compact format as hand cards.
    for (const SkillCard* card : skillDeck.getDeck()) {
        if (card == nullptr) {
            continue;
        }

        std::ostringstream cardLine;
        cardLine << card->getTypeName();

        const int value = card->getSkillValue();
        const int duration = card->getRemainingDuration();
        const bool hasValue = value != 0;
        const bool hasDuration = duration != 0;
        if (hasDuration || card->getTypeName() == "DiscountCard") {
            cardLine << ' ' << value << ' ' << duration;
        } else if (hasValue) {
            cardLine << ' ' << value;
        }

        state.deckState.push_back(cardLine.str());
    }

    // Copy log from TransactionLogger to state
    state.log = logger.getAll();

    // Build tile names from board for GUI display
    for (int i = 0; i < board.getSize(); ++i) {
        const Tile* tile = board.getTile(i);
        if (tile != nullptr) {
            state.tileNames[i] = tile->getName();
        }
    }

    return state;
}

Board& GameEngine::getBoard() {
    return board;
}

// Helper function to create skill card from CardState
static SkillCard* createSkillCardFromState(const CardState& cardState) {
    if (cardState.type == "DemolitionCard") {
        return new DemolitionCard();
    } else if (cardState.type == "DiscountCard") {
        return new DiscountCard(cardState.value);
    } else if (cardState.type == "LassoCard") {
        return new LassoCard();
    } else if (cardState.type == "MoveCard") {
        return new MoveCard(cardState.value);
    } else if (cardState.type == "ShieldCard") {
        return new ShieldCard();
    } else if (cardState.type == "TeleportCard") {
        return new TeleportCard(cardState.value);
    }
    return nullptr;
}

// Helper function to convert PlayerStatus to string
static std::string playerStatusToString(PlayerStatus status) {
    switch (status) {
        case PlayerStatus::ACTIVE: return "ACTIVE";
        case PlayerStatus::JAILED: return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
        default: return "ACTIVE";
    }
}

// Helper function to convert string to PlayerStatus
static PlayerStatus stringToPlayerStatus(const std::string& status) {
    if (status == "JAILED") return PlayerStatus::JAILED;
    if (status == "BANKRUPT") return PlayerStatus::BANKRUPT;
    return PlayerStatus::ACTIVE;
}

void GameEngine::loadGame(const std::string& file) {
    std::cout << "[INIT] Starting New Game\n";
    std::cout << "[INIT] Generating default board\n";
    this->board.generateDefaultBoard();

    std::cout << "[INIT] Loading game from file: " << file << "\n";

    SaveLoadManager saveLoadManager;
    GameState state = saveLoadManager.load(file);

    // Clear existing players
    players.clear();

    // Create players based on saved state
    for (const PlayerState& ps : state.players) {
        Player* player;

        if (ps.isBot) {
            // Create BotPlayer
            player = new BotPlayer (ps.username, ps.balance);
            std::cout << "[INFO] Loaded bot player: " << ps.username << "\n";
        } else {
            // Create regular Player
            player = new Player(ps.username, ps.balance);
            std::cout << "[INFO] Loaded player: " << ps.username << "\n";
        }

        // Set player state
        player->setPosition(board.getTileByCode(ps.positionCode) != nullptr ?
            board.getTileByCode(ps.positionCode)->getIndex() : 0);

        // Set status based on saved state
        if (ps.status == "JAILED") {
            player->enterJail();
        } else if (ps.status == "BANKRUPT") {
            player->setBankruptStatus();
        }

        // Add skill cards to hand
        for (const CardState& cardState : ps.hand) {
            SkillCard* card = createSkillCardFromState(cardState);
            if (card != nullptr) {
                player->drawSCard(card);
            }
        }

        players.push_back(std::move(player));
    }

    // Set up turn manager
    turnmgr = TurnManager(state.maxTurn);
    turnmgr.setTurnOrder(this->getPlayers());

    // Set current turn and active player index from saved state
    turnmgr.setCurrentTurn(state.currentTurn);
    turnmgr.setActivePlayerIndex(state.activePlayerIdx);

    // Copy log from state to TransactionLogger
    for (const auto& entry : state.log) {
        logger.log(entry.turn, entry.username, entry.actionType, entry.detail);
    }

    // Log the load action itself
    logger.log(turnmgr.getCurrentTurn(), "SYSTEM", "LOAD", "Game loaded from " + file);

    std::cout << "[INFO] Game loaded successfully!\n";
    std::cout << "[INFO] Current turn: " << state.currentTurn << "/" << state.maxTurn << "\n";
    if (state.activePlayerIdx >= 0 && state.activePlayerIdx < static_cast<int>(players.size())) {
        std::cout << "[INFO] Active player: " << players[state.activePlayerIdx]->getUsername() << "\n";
    }
}

void GameEngine::saveGame(const std::string& file) {
    std::cout << "[INFO] Saving game to file: " << file << "\n";

    // Log the save action first so it is included in saved transaction history.
    logger.log(turnmgr.getCurrentTurn(), "SYSTEM", "SAVE", "Game saved to " + file);

    // Build complete snapshot (players, turn order, properties, deck, and logs).
    GameState state = buildGameState();

    // Save using SaveLoadManager
    SaveLoadManager saveLoadManager;
    saveLoadManager.save(state, file);

    std::cout << "[INFO] Game saved successfully!\n";
}
