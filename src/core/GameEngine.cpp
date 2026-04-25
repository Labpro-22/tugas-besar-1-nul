#include "core/GameEngine.hpp"

#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/ConfigLoader.hpp"
#include "core/SaveLoadManager.hpp"
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

#include <limits>
#include <fstream>
#include <map>
#include <cstdlib>
#include <ctime>
#include <random>

GameEngine::GameEngine(int size)
    : board(Board{size})
        , turnmgr(TurnManager{})
        , chanceDeck(CardDeck<ChanceCard>{})
        , communityDeck(CardDeck<CommunityChestCard>{})
        , skillDeck(CardDeck<SkillCard>{})
        , status(GameStatus::NOT_STARTED)
        , activeConfig(Config{})
        , players{std::vector<Player*>{}} {
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
        std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1) << ": " << currentPlayer->getUsername() << " ===\n";

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
}

#if NIMONSPOLI_HAS_RAYLIB
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

static void RenderGameFrame(GUIRenderer& renderer, GameEngine& engine,
                            TurnManager& turnmgr, Board& board,
                            Dice& gameDice, int& animatingDiceFrames) {
    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer == nullptr) return;

    TurnContext ctx(*currentPlayer, gameDice, board, engine);
    GameState state = engine.buildGameState();

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
    float controlsY = snapshot.rightPanel.y + snapshot.rightPanel.height - 240;

    // Draw turn info panel
    DrawRectangle(10, 10, 340, 100, Color{30, 30, 40, 230});
    DrawRectangleLinesEx(Rectangle{10.0f, 10.0f, 340.0f, 100.0f}, 2.0f, Color{100, 100, 120, 255});
    DrawText(TextFormat("TURN %d / %d", turnmgr.getCurrentTurn() + 1,
                        turnmgr.getMaxTurn() > 0 ? turnmgr.getMaxTurn() : 999),
             20, 18, 22, Color{255, 220, 100, 255});
    DrawText("Current Player:", 20, 48, 14, LIGHTGRAY);
    DrawText(TextFormat("%s", currentPlayer->getUsername().c_str()),
             150, 44, 24,
             currentPlayer->isBot() ? Color{255, 180, 80, 255} : Color{100, 255, 150, 255});
    DrawText(currentPlayer->isBot() ? "[BOT PLAYER]" : "[HUMAN PLAYER]",
             20, 78, 14, GRAY);

    // Draw balance info
    DrawText(TextFormat("Balance: M%d", currentPlayer->getBalance()),
             150, 78, 14, Color{255, 220, 100, 255});

    // Draw control buttons
    bool canRoll = !ctx.hasRolled && !currentPlayer->isInJail();
    bool canEndTurn = ctx.canEndTurn();
    renderer.DrawGameControls(controlsX, controlsY, canRoll, canEndTurn);

    // Draw dice animation status
    if (animatingDiceFrames > 0) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        DrawRectangle(sw / 2 - 100, sh / 2 - 30, 200, 40, Color{0, 0, 0, 180});
        DrawText("Rolling Dice...", sw / 2 - 80, sh / 2 - 20, 20, WHITE);
    }

    // Draw message area
    float consoleY = static_cast<float>(GetScreenHeight()) - 100.0f;
    DrawRectangle(10, static_cast<int>(consoleY), 600, 90, Color{20, 20, 25, 230});
    DrawRectangleLinesEx(Rectangle{10.0f, consoleY, 600.0f, 90.0f}, 1.0f, Color{80, 80, 90, 255});
    DrawText("Console Output:", 20, GetScreenHeight() - 92, 12, Color{150, 150, 160, 255});
    DrawText("Press keys or click buttons to play", 20, GetScreenHeight() - 75, 14, LIGHTGRAY);
    DrawText("[R] Roll  [E] End Turn  [P] Profile  [B] Build  [M] Mortgage  [S] Save  [F12] Screenshot  [ESC] Exit",
             20, GetScreenHeight() - 55, 12, GRAY);

    EndDrawing();
}

static bool HandleGameInput(GUIRenderer& renderer, GameEngine& engine,
                            TurnManager& turnmgr, Board& board,
                            Dice& gameDice, int& animatingDiceFrames,
                            Command& cmd, bool& goNext) {
    auto snapshot = renderer.InspectBoardDisplay();
    float controlsX = snapshot.rightPanel.x + 15;
    float controlsY = snapshot.rightPanel.y + snapshot.rightPanel.height - 240;
    const float buttonWidth = 120.0f;
    const float buttonHeight = 40.0f;
    const float spacing = 10.0f;

    bool rollClicked = renderer.IsButtonClicked(controlsX, controlsY, buttonWidth, buttonHeight);
    bool endTurnClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY, buttonWidth, buttonHeight);
    bool profileClicked = renderer.IsButtonClicked(controlsX, controlsY + buttonHeight + spacing, buttonWidth, buttonHeight);
    bool buildClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + buttonHeight + spacing, buttonWidth, buttonHeight);
    bool mortgageClicked = renderer.IsButtonClicked(controlsX, controlsY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool saveClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool helpClicked = renderer.IsButtonClicked(controlsX, controlsY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    bool exitClicked = renderer.IsButtonClicked(controlsX + buttonWidth + spacing, controlsY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);

    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer == nullptr) return true;

    TurnContext ctx(*currentPlayer, gameDice, board, engine);

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
            if (goNext) {
                turnmgr.nextTurn(ctx);
                return true; // Frame handled, skip rest
            }
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
    else if (IsKeyPressed(KEY_S) || saveClicked) {
        try { cmd.parse("SAVE"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
    }
    else if (IsKeyPressed(KEY_H) || helpClicked) {
        try { cmd.parse("HELP"); cmd.dispatch(ctx); }
        catch (const std::exception& exc) { std::cout << exc.what() << "\n"; }
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

    for (int i = 1; i <= GUIRenderer::kBoardTileCount; ++i) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "T%02d", i);
        config.boardVisual.tileCodesInOrder.push_back(buf);
    }
    config.boardVisual.tileTextureByCode = GUIRenderer::BuildDefaultTileVisualMap();

    if (!renderer.Initialize(config)) {
        std::cerr << "[ERROR] Failed to initialize GUI renderer\n";
        std::cout << "Falling back to console mode...\n";
        run();
        return;
    }

    // Show splash screen while console menu is active
    // Render frames for a moment to show the window is alive
    double splashStart = GetTime();
    while (GetTime() - splashStart < 1.5 && !WindowShouldClose()) {
        RenderSplashScreen(renderer, "Starting game");
    }

    // Console-based menu (window exists but console handles input)
    this->printBanner();
    this->startMenu();

    // Show splash again briefly after menu
    splashStart = GetTime();
    while (GetTime() - splashStart < 1.0 && !WindowShouldClose()) {
        RenderSplashScreen(renderer, "Loading board");
    }

    Dice gameDice;
    Command cmd;
    bool goNext = false;
    int animatingDiceFrames = 0;

    // Main game loop
    while (!turnmgr.isGameOver()) {
        if (WindowShouldClose()) {
            break;
        }

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

            // Start dice animation BEFORE executing turn
            renderer.SetDiceValues(1, 1);
            renderer.StartDiceRoll(
                (std::rand() % 6) + 1,
                (std::rand() % 6) + 1
            );
            animatingDiceFrames = 90;

            // Play animation for a moment
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

            // After bot turn, set actual dice values and animate to final
            if (gameDice.getDie1() > 0 && gameDice.getDie2() > 0) {
                renderer.StartDiceRoll(gameDice.getDie1(), gameDice.getDie2());
                animatingDiceFrames = 90;
            }

            // Render a few frames to show the final dice state
            double postTurnStart = GetTime();
            while (GetTime() - postTurnStart < 0.5 && !WindowShouldClose()) {
                RenderGameFrame(renderer, *this, turnmgr, board, gameDice, animatingDiceFrames);
            }

            if (goNext) {
                turnmgr.nextTurn(ctx);
            }
            continue;
        }

        // Human player turn - render continuously and handle input
        TurnContext ctx(*currentPlayer, gameDice, board, *this);

        // Only print turn info once per human turn (not every frame)
        static Player* lastPrintedPlayer = nullptr;
        static int lastPrintedTurn = -1;
        if (currentPlayer != lastPrintedPlayer || turnmgr.getCurrentTurn() != lastPrintedTurn) {
            std::cout << "\n=== Giliran " << (turnmgr.getCurrentTurn() + 1)
                      << ": " << currentPlayer->getUsername() << " ===\n";
            lastPrintedPlayer = currentPlayer;
            lastPrintedTurn = turnmgr.getCurrentTurn();
        }

        // Render frame
        RenderGameFrame(renderer, *this, turnmgr, board, gameDice, animatingDiceFrames);

        // Decrement animation counter
        if (animatingDiceFrames > 0) {
            animatingDiceFrames--;
        }

        // Handle input
        bool shouldExit = HandleGameInput(renderer, *this, turnmgr, board,
                                          gameDice, animatingDiceFrames, cmd, goNext);
        if (shouldExit) {
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
                    }
                } catch (const std::exception& exc) {
                    std::cout << exc.what() << "\n";
                }
            }
        }
    }

    std::cout << "\n=== Permainan Selesai! ===\n";
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
    turnmgr.setTurnOrder(this->getPlayers());

    // Trigger tile onLanded
    baseTile->onLanded(ctx);

    // Small delay for readability
    std::cout << "[BOT] Giliran " << bot->getUsername() << " selesai.\n";

    return true; // End bot's turn
}

void GameEngine::loadGameFromSave() {
    clearPlayers();

    std::cout << "[INFO] Loading Game from Save File...\n";
    std::cout << "Masukkan nama file save game: ";
    std::string filename;
    std::getline(std::cin, filename);

    if (filename.empty()) {
        std::cout << "[ERROR] Nama file tidak boleh kosong. Kembali ke menu...\n";
        return;
    }

    try {
        this->board.generateDefaultBoard();
        loadGame(filename);
    } catch (const std::exception& e) {
        std::cout << "[ERROR] Gagal load game: " << e.what() << "\n";
        std::cout << "Menggunakan konfigurasi default...\n";
        // Fallback ke new game jika load gagal
        this->newGame();
    }
}
void GameEngine::newGame() {
    std::cout << "\n[INFO] Starting New Game...\n";
    std::cout << "[INFO] Generating default board\n";
    this->board.generateDefaultBoard();

    int maxTurns;
    std::cout << "[INFO] Querying max turns\n";
    std::cout << "Enter max turn (-1 for unlimited turns)\n";
    std::cout << "> ";
    std::cin >> maxTurns;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    this->turnmgr = TurnManager(maxTurns);

    int numPlayers;
    std::cout << "\n";
    std::cout << "[INFO] Querying players\n";
    std::cout << "Enter player amount (2-6)\n";
    std::cout << "> ";
    std::cin >> numPlayers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int numBots;
    std::cout << "Enter number of bots (0-" << numPlayers << "): ";
    std::cin >> numBots;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (numBots < 0) numBots = 0;
    if (numBots > numPlayers) numBots = numPlayers;

    int numHumans = numPlayers - numBots;

    // Create human players
    for (int i = 0; i < numHumans; ++i) {
        std::string username;
        std::cout << "Enter player " << (i + 1) << "'s name: ";
        std::getline(std::cin, username);
        players.push_back(new Player(username, 1500)); // saldo awal 1500
        std::cout << "[INFO] Added human player: " << username << "\n";
    }

    // Create bot players
    for (int i = 0; i < numBots; ++i) {
        std::string botName = "Bot" + std::to_string(i + 1);
        players.push_back(new BotPlayer(botName, 1500));
        std::cout << "[INFO] Added bot player: " << botName << "\n";
    }

    // Give each player 1 skill card at the start of the game
    std::cout << "\n[INFO] Distributing initial skill cards to all players...\n";
    for (Player* player : players) {
        if (player != nullptr) {
            giveRandomSkillCardTo(*player);
        }
    }

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
        std::cout << "Enter a choice (1-4): ";
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
            this->newGame();
            break;
        } else if (choice == 2) {
            this->loadGameFromSave();
            break;
        } else if (choice == 3) {
            break;
        } else if (choice == 4) {
            std::cout << "Thank you for playing!\n";
            exit(0);
            break;
        } else {
            std::cout << "Invalid choice. Please retry\n\n";
        }
    }
}

TurnManager& GameEngine::getTurnManager() {
    return turnmgr;
}

int GameEngine::getGoSalary() const {
    return (activeConfig.special.goSalary > 0) ? activeConfig.special.goSalary : 200;
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

        // Determine property type
        if (dynamic_cast<StreetProperty*>(prop) != nullptr) {
            propState.type = "street";
            StreetProperty* street = dynamic_cast<StreetProperty*>(prop);
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
        }

        // Get status
        switch (prop->getStatus()) {
            case PropertyStatus::OWNED:
                propState.status = "owned";
                break;
            case PropertyStatus::MORTGAGED:
                propState.status = "mortgage";
                break;
            default:
                propState.status = "bank";
                break;
        }

        // Festival info
        propState.festivalMult = prop->getFestivalMultiplier();
        propState.festivalDur = prop->getFestivalDuration();

        state.properties.push_back(propState);
    }

    // Copy log from TransactionLogger to state
    state.log = logger.getAll();

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
    std::cout << "[INFO] Loading game from file: " << file << "\n";

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

    GameState state;
    state.currentTurn = turnmgr.getCurrentTurn();
    state.maxTurn = turnmgr.getMaxTurn();

    // Build player states
    for (const auto& player : players) {
        PlayerState ps;
        ps.username = player->getUsername();
        ps.balance = player->getBalance();

        // Get position code from board
        Tile* tile = board.getTile(player->getPosition());
        ps.positionCode = tile != nullptr ? tile->getCode() : "GO";

        // Get status
        ps.status = playerStatusToString(player->getStatus());

        // Check if bot
        ps.isBot = player->isBot();

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

    // Set active player index
    Player* currentPlayer = turnmgr.getCurrentPlayer();
    if (currentPlayer != nullptr) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] == currentPlayer) {
                state.activePlayerIdx = static_cast<int>(i);
                break;
            }
        }
    }

    // Copy log from TransactionLogger to state
    state.log = logger.getAll();

    // Log the save action itself
    logger.log(turnmgr.getCurrentTurn(), "SYSTEM", "SAVE", "Game saved to " + file);

    // Save using SaveLoadManager
    SaveLoadManager saveLoadManager;
    saveLoadManager.save(state, file);

    std::cout << "[INFO] Game saved successfully!\n";
}
