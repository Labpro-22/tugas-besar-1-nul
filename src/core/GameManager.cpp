// // ini gada bedanya sm gameengine brok, nnt gw pindah ke ge dah

// #include "core/GameManager.hpp"
// #include "player/Player.hpp"
// #include "player/BotPlayer.hpp"
// #include "property/Property.hpp"
// #include "property/StreetProperty.hpp"
// #include "property/RailroadProperty.hpp"
// #include "property/UtilityProperty.hpp"
// #include "tile/Tile.hpp"
// #include "tile/PropertyTile.hpp"
// #include "tile/ActionTile.hpp"
// #include "card/SkillCard.hpp"
// #include "card/MoveCard.hpp"
// #include "card/ShieldCard.hpp"
// #include "card/DiscountCard.hpp"
// #include "card/TeleportCard.hpp"
// #include "card/LassoCard.hpp"
// #include "card/DemolitionCard.hpp"
// #include "core/BankruptcyManager.hpp"
// #include "core/SaveLoadManager.hpp"
// #include "config/PropertyData.hpp"
// #include "states/GameState.hpp"
// #include "states/PlayerState.hpp"
// #include "states/PropertyState.hpp"
// #include "states/CardState.hpp"
// #include "states/LogEntry.hpp"

// #include <iostream>
// #include <algorithm>
// #include <random>
// #include <ctime>
// #include <fstream>
// #include <sstream>

// GameManager::GameManager() 
//     : status(GameStatus::NOT_STARTED),
//       board(nullptr),
//       currentTurnNumber(1),
//       currentPlayerIndex(0),
//       currentContext(nullptr),
//       consecutiveDoubles(0) {
//     std::srand(static_cast<unsigned int>(std::time(nullptr)));
// }

// GameManager::~GameManager() {
//     // Clean up players
//     for (Player* player : players) {
//         delete player;
//     }
    
//     // Clean up tiles
//     for (Tile* tile : tiles) {
//         delete tile;
//     }
    
//     // Clean up board
//     delete board;
    
//     // Clean up skill cards
//     for (SkillCard* card : skillDeck) {
//         delete card;
//     }
//     for (SkillCard* card : skillDiscardPile) {
//         delete card;
//     }
    
//     delete currentContext;
// }

// void GameManager::initialize(Config cfg) {
//     config = cfg;
//     initializeBoard();
//     initializeSkillDeck();
// }

// void GameManager::initializeBoard() {
//     // Create board with size from config or default 40
//     board = new Board(40);
    
//     // Build all 40 tiles according to the specification
//     buildTiles();
// }

// void GameManager::buildTiles() {
//     // Build all 40 tiles according to the specification
//     // Index 0-39 (0-based) corresponds to position 1-40 (1-based in spec)
    
//     // Position 1: GO (index 0)
//     tiles.push_back(new GoTile(0, "GO", "Petak Mulai"));
    
//     // Position 2: GRT - Garut (Street - Coklat)
//     tiles.push_back(new StreetTile(1, new StreetProperty(
//         "GRT", "Garut", 60, 30, "Coklat", 50, 50, 
//         std::vector<int>{2, 10, 30, 90, 160, 250}
//     )));
    
//     // Position 3: DNU - Dana Umum
//     tiles.push_back(new CardTile(2, "DNU", "Dana Umum", false));
    
//     // Position 4: TSK - Tasikmalaya (Street - Coklat)
//     tiles.push_back(new StreetTile(3, new StreetProperty(
//         "TSK", "Tasikmalaya", 60, 30, "Coklat", 50, 50, 
//         std::vector<int>{4, 20, 60, 180, 320, 450}
//     )));
    
//     // Position 5: PPH - Pajak Penghasilan
//     tiles.push_back(new TaxTile(4, "PPH", "Pajak Penghasilan", TaxType::PPH));
    
//     // Position 6: GBR - Stasiun Gambir (Railroad)
//     tiles.push_back(new RailroadTile(5, new RailroadProperty(
//         "GBR", "Stasiun Gambir", 200, 100, 
//         std::map<int, int>{{1, 25}, {2, 50}, {3, 100}, {4, 200}}
//     )));
    
//     // Position 7: BGR - Bogor (Street - Biru Muda)
//     tiles.push_back(new StreetTile(6, new StreetProperty(
//         "BGR", "Bogor", 100, 50, "Biru Muda", 50, 50, 
//         std::vector<int>{6, 30, 90, 270, 400, 550}
//     )));
    
//     // Position 8: FES - Festival
//     tiles.push_back(new FestivalTile(7, "FES", "Festival"));
    
//     // Position 9: DPK - Depok (Street - Biru Muda)
//     tiles.push_back(new StreetTile(8, new StreetProperty(
//         "DPK", "Depok", 100, 50, "Biru Muda", 50, 50, 
//         std::vector<int>{6, 30, 90, 270, 400, 550}
//     )));
    
//     // Position 10: BKS - Bekasi (Street - Biru Muda)
//     tiles.push_back(new StreetTile(9, new StreetProperty(
//         "BKS", "Bekasi", 120, 60, "Biru Muda", 50, 50, 
//         std::vector<int>{8, 40, 100, 300, 450, 600}
//     )));
    
//     // Position 11: PEN - Penjara
//     tiles.push_back(new JailTile(10, "PEN", "Penjara"));
    
//     // Position 12: MGL - Magelang (Street - Pink)
//     tiles.push_back(new StreetTile(11, new StreetProperty(
//         "MGL", "Magelang", 140, 70, "Pink", 100, 100, 
//         std::vector<int>{10, 50, 150, 450, 625, 750}
//     )));
    
//     // Position 13: PLN - PLN (Utility)
//     tiles.push_back(new UtilityTile(12, new UtilityProperty(
//         "PLN", "PLN", 150, 75, 
//         std::map<int, int>{{1, 4}, {2, 10}} // 1 utility = 4x dadu, 2 utility = 10x dadu
//     )));
    
//     // Position 14: SOL - Solo (Street - Pink)
//     tiles.push_back(new StreetTile(13, new StreetProperty(
//         "SOL", "Solo", 140, 70, "Pink", 100, 100, 
//         std::vector<int>{10, 50, 150, 450, 625, 750}
//     )));
    
//     // Position 15: YOG - Yogyakarta (Street - Pink)

//     tiles.push_back(new StreetTile(14, new StreetProperty(

//     tiles.push_back(new StreetTile(14, *new StreetProperty(

//         "YOG", "Yogyakarta", 160, 80, "Pink", 100, 100, 
//         std::vector<int>{12, 60, 180, 500, 700, 900}
//     )));
    
//     // Position 16: STB - Stasiun Bandung (Railroad)

//     tiles.push_back(new RailroadTile(15, new RailroadProperty(

//     tiles.push_back(new RailroadTile(15, *new RailroadProperty(

//         "STB", "Stasiun Bandung", 200, 100, 
//         std::map<int, int>{{1, 25}, {2, 50}, {3, 100}, {4, 200}}
//     )));
    
//     // Position 17: MAL - Malang (Street - Orange)

//     tiles.push_back(new StreetTile(16, new StreetProperty(

//     tiles.push_back(new StreetTile(16, *new StreetProperty(

//         "MAL", "Malang", 180, 90, "Orange", 100, 100, 
//         std::vector<int>{14, 70, 200, 550, 750, 950}
//     )));
    
//     // Position 18: DNU - Dana Umum
//     tiles.push_back(new CardTile(17, "DNU", "Dana Umum", false));
    
//     // Position 19: SMG - Semarang (Street - Orange)

//     tiles.push_back(new StreetTile(18, new StreetProperty(

//     tiles.push_back(new StreetTile(18, *new StreetProperty(

//         "SMG", "Semarang", 180, 90, "Orange", 100, 100, 
//         std::vector<int>{14, 70, 200, 550, 750, 950}
//     )));
    
//     // Position 20: SBY - Surabaya (Street - Orange)

//     tiles.push_back(new StreetTile(19, new StreetProperty(

//     tiles.push_back(new StreetTile(19, *new StreetProperty(

//         "SBY", "Surabaya", 200, 100, "Orange", 100, 100, 
//         std::vector<int>{16, 80, 220, 600, 800, 1000}
//     )));
    
//     // Position 21: BBP - Bebas Parkir
//     tiles.push_back(new FreeParkingTile(20, "BBP", "Bebas Parkir"));
    
//     // Position 22: MKS - Makassar (Street - Merah)

//     tiles.push_back(new StreetTile(21, new StreetProperty(

//     tiles.push_back(new StreetTile(21, *new StreetProperty(

//         "MKS", "Makassar", 220, 110, "Merah", 150, 150, 
//         std::vector<int>{18, 90, 250, 700, 875, 1050}
//     )));
    
//     // Position 23: KSP - Kesempatan
//     tiles.push_back(new CardTile(22, "KSP", "Kesempatan", true));
    
//     // Position 24: BLP - Balikpapan (Street - Merah)

//     tiles.push_back(new StreetTile(23, new StreetProperty(

//     tiles.push_back(new StreetTile(23, *new StreetProperty(

//         "BLP", "Balikpapan", 220, 110, "Merah", 150, 150, 
//         std::vector<int>{18, 90, 250, 700, 875, 1050}
//     )));
    
//     // Position 25: MND - Manado (Street - Merah)

//     tiles.push_back(new StreetTile(24, new StreetProperty(

//     tiles.push_back(new StreetTile(24, *new StreetProperty(

//         "MND", "Manado", 240, 120, "Merah", 150, 150, 
//         std::vector<int>{20, 100, 300, 750, 925, 1100}
//     )));
    
//     // Position 26: TUG - Stasiun Tugu (Railroad)

//     tiles.push_back(new RailroadTile(25, new RailroadProperty(

//     tiles.push_back(new RailroadTile(25, *new RailroadProperty(

//         "TUG", "Stasiun Tugu", 200, 100, 
//         std::map<int, int>{{1, 25}, {2, 50}, {3, 100}, {4, 200}}
//     )));
    
//     // Position 27: PLB - Palembang (Street - Kuning)

//     tiles.push_back(new StreetTile(26, new StreetProperty(

//     tiles.push_back(new StreetTile(26, *new StreetProperty(

//         "PLB", "Palembang", 260, 130, "Kuning", 150, 150, 
//         std::vector<int>{22, 110, 330, 800, 975, 1150}
//     )));
    
//     // Position 28: PKB - Pekanbaru (Street - Kuning)

//     tiles.push_back(new StreetTile(27, new StreetProperty(

//     tiles.push_back(new StreetTile(27, *new StreetProperty(

//         "PKB", "Pekanbaru", 260, 130, "Kuning", 150, 150, 
//         std::vector<int>{22, 110, 330, 800, 975, 1150}
//     )));
    
//     // Position 29: PAM - PAM (Utility)

//     tiles.push_back(new UtilityTile(28, new UtilityProperty(

//     tiles.push_back(new UtilityTile(28, *new UtilityProperty(

//         "PAM", "PAM", 150, 75, 
//         std::map<int, int>{{1, 4}, {2, 10}}
//     )));
    
//     // Position 30: MED - Medan (Street - Kuning)

//     tiles.push_back(new StreetTile(29, new StreetProperty(

//     tiles.push_back(new StreetTile(29, *new StreetProperty(

//         "MED", "Medan", 280, 140, "Kuning", 150, 150, 
//         std::vector<int>{24, 120, 360, 850, 1025, 1200}
//     )));
    
//     // Position 31: PPJ - Pergi ke Penjara
//     tiles.push_back(new GoToJailTile(30, "PPJ", "Pergi ke Penjara"));
    
//     // Position 32: BDG - Bandung (Street - Hijau)

//     tiles.push_back(new StreetTile(31, new StreetProperty(

//     tiles.push_back(new StreetTile(31, *new StreetProperty(

//         "BDG", "Bandung", 300, 150, "Hijau", 200, 200, 
//         std::vector<int>{26, 130, 390, 900, 1100, 1275}
//     )));
    
//     // Position 33: DEN - Denpasar (Street - Hijau)

//     tiles.push_back(new StreetTile(32, new StreetProperty(

//     tiles.push_back(new StreetTile(32, *new StreetProperty(

//         "DEN", "Denpasar", 300, 150, "Hijau", 200, 200, 
//         std::vector<int>{26, 130, 390, 900, 1100, 1275}
//     )));
    
//     // Position 34: FES - Festival
//     tiles.push_back(new FestivalTile(33, "FES", "Festival"));
    
//     // Position 35: MTR - Mataram (Street - Hijau)

//     tiles.push_back(new StreetTile(34, new StreetProperty(

//     tiles.push_back(new StreetTile(34, *new StreetProperty(

//         "MTR", "Mataram", 320, 160, "Hijau", 200, 200, 
//         std::vector<int>{28, 150, 450, 1000, 1200, 1400}
//     )));
    
//     // Position 36: GUB - Stasiun Gubeng (Railroad)

//     tiles.push_back(new RailroadTile(35, new RailroadProperty(

//     tiles.push_back(new RailroadTile(35, *new RailroadProperty(

//         "GUB", "Stasiun Gubeng", 200, 100, 
//         std::map<int, int>{{1, 25}, {2, 50}, {3, 100}, {4, 200}}
//     )));
    
//     // Position 37: KSP - Kesempatan
//     tiles.push_back(new CardTile(36, "KSP", "Kesempatan", true));
    
//     // Position 38: JKT - Jakarta (Street - Biru Tua)

//     tiles.push_back(new StreetTile(37, new StreetProperty(

//     tiles.push_back(new StreetTile(37, *new StreetProperty(

//         "JKT", "Jakarta", 350, 175, "Biru Tua", 200, 200, 
//         std::vector<int>{35, 175, 500, 1100, 1300, 1500}
//     )));
    
//     // Position 39: PBM - Pajak Barang Mewah
//     tiles.push_back(new TaxTile(38, "PBM", "Pajak Barang Mewah", TaxType::PBM));
    
//     // Position 40: IKN - Ibu Kota Nusantara (Street - Biru Tua)

//     tiles.push_back(new StreetTile(39, new StreetProperty(

//     tiles.push_back(new StreetTile(39, *new StreetProperty(

//         "IKN", "Ibu Kota Nusantara", 400, 200, "Biru Tua", 200, 200, 
//         std::vector<int>{50, 200, 600, 1400, 1700, 2000}
//     )));
// }

// void GameManager::initializeSkillDeck() {
//     // Create skill cards according to spec:
//     // 4 MoveCards, 3 DiscountCards, 2 ShieldCards, 2 TeleportCards, 2 LassoCards, 2 DemolitionCards
    
//     // Random number generator for card parameters
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> moveDist(1, 10);
//     std::uniform_int_distribution<> discountDist(10, 50);
//     std::uniform_int_distribution<> teleportDist(0, 39);
    
//     // MoveCards (4) - random steps 1-10
//     for (int i = 0; i < 4; ++i) {
//         skillDeck.push_back(new MoveCard(moveDist(gen)));
//     }
    
//     // DiscountCards (3) - random discount 10-50%
//     for (int i = 0; i < 3; ++i) {
//         skillDeck.push_back(new DiscountCard(discountDist(gen)));
//     }
    
//     // ShieldCards (2)
//     for (int i = 0; i < 2; ++i) {
//         skillDeck.push_back(new ShieldCard());
//     }
    
//     // TeleportCards (2) - random position
//     for (int i = 0; i < 2; ++i) {
//         skillDeck.push_back(new TeleportCard(teleportDist(gen)));
//     }
    
//     // LassoCards (2)
//     for (int i = 0; i < 2; ++i) {
//         skillDeck.push_back(new LassoCard());
//     }
    
//     // DemolitionCards (2)
//     for (int i = 0; i < 2; ++i) {
//         skillDeck.push_back(new DemolitionCard());
//     }
    
//     shuffleSkillDeck();
// }

// void GameManager::shuffleSkillDeck() {
//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(skillDeck.begin(), skillDeck.end(), g);
// }

// void GameManager::setupNewGame(int numPlayers, const std::vector<std::string>& usernames) {
//     // Create players
//     for (int i = 0; i < numPlayers; ++i) {
//         Player* player = new Player(usernames[i], config.misc.startingBalance);
//         player->setBoardSizeSource(&board->getSizeRef());
//         players.push_back(player);
//     }
    
//     // Randomize turn order
//     turnOrder.clear();
//     for (int i = 0; i < numPlayers; ++i) {
//         turnOrder.push_back(i);
//     }
//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(turnOrder.begin(), turnOrder.end(), g);
    
//     currentPlayerIndex = 0;
//     currentTurnNumber = 1;
// }

// void GameManager::setupNewGameWithBots(const std::vector<std::string>& humanPlayerNames, 
//                                        int numBots, int botDifficulty) {
//     int totalPlayers = humanPlayerNames.size() + numBots;
    
//     // Create human players
//     for (const std::string& name : humanPlayerNames) {
//         Player* player = new Player(name, config.misc.startingBalance);
//         player->setBoardSizeSource(&board->getSizeRef());
//         players.push_back(player);
//     }
    
//     // Create bot players with predefined names
//     std::vector<std::string> botNames = {"AlphaBot", "BetaBot", "GammaBot", "DeltaBot"};
//     for (int i = 0; i < numBots; ++i) {
//         std::string botName = botNames[i % botNames.size()];
//         // Add number suffix if needed
//         if (numBots > botNames.size() || i > 0) {
//             botName += std::to_string(i + 1);
//         }
        
//         BotPlayer* bot = new BotPlayer(botName, config.misc.startingBalance, botDifficulty);
//         bot->setBoardSizeSource(&board->getSizeRef());
//         players.push_back(bot);
//     }
    
//     // Randomize turn order
//     turnOrder.clear();
//     for (int i = 0; i < totalPlayers; ++i) {
//         turnOrder.push_back(i);
//     }
//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(turnOrder.begin(), turnOrder.end(), g);
    
//     currentPlayerIndex = 0;
//     currentTurnNumber = 1;
// }

// void GameManager::setupLoadedGame(const std::vector<std::string>& usernames,
//                                   const std::vector<int>& balances,
//                                   const std::vector<int>& positions,
//                                   const std::vector<std::string>& statuses,
//                                   int currentTurn, int currentPlayerIdx) {
//     int numPlayers = usernames.size();
    
//     for (int i = 0; i < numPlayers; ++i) {
//         Player* player = new Player(usernames[i], balances[i]);
//         player->setBoardSizeSource(&board->getSizeRef());
        
//         // Set position
//         for (int j = 0; j < positions[i]; ++j) {
//             player->move(1);
//         }
        
//         // Set status
//         if (statuses[i] == "JAILED") {
//             player->enterJail();
//         } else if (statuses[i] == "BANKRUPT") {
//             player->setBankruptStatus();
//         }
        
//         players.push_back(player);
//         turnOrder.push_back(i);
//     }
    
//     currentTurnNumber = currentTurn;
//     currentPlayerIndex = currentPlayerIdx;
// }

// void GameManager::startGame() {
//     status = GameStatus::RUNNING;
//     std::cout << "=== PERMAINAN DIMULAI ===" << std::endl;
//     std::cout << "Jumlah pemain: " << players.size() << std::endl;
//     std::cout << "Urutan giliran:" << std::endl;
//     for (size_t i = 0; i < turnOrder.size(); ++i) {
//         std::cout << "  " << (i + 1) << ". " << players[turnOrder[i]]->getUsername() << std::endl;
//     }
//     std::cout << "====" << std::endl;
// }

// void GameManager::runGameLoop() {
//     while (status == GameStatus::RUNNING) {
//         processTurn();
        
//         if (!nextTurn()) {
//             break;
//         }
//     }
// }

// void GameManager::processTurn() {
//     Player* currentPlayer = getCurrentPlayer();
//     if (currentPlayer == nullptr || currentPlayer->getStatus() == PlayerStatus::BANKRUPT) {
//         return;
//     }
    
//     std::cout << "\n=== TURN " << currentTurnNumber << " / " << config.misc.maxTurn 
//               << " ===" << std::endl;
//     std::cout << "Giliran: " << currentPlayer->getUsername() << std::endl;
//     std::cout << "Posisi: " << currentPlayer->getPosition() << std::endl;
//     std::cout << "Uang: M" << currentPlayer->getBalance() << std::endl;
    
//     // Create turn context
//     delete currentContext;
//     currentContext = new TurnContext(currentPlayer, board, currentTurnNumber, 
//                                      config.misc.maxTurn, players);
//     currentContext->setLogger(&logger);
    
//     // Draw skill card at start of turn
//     drawSkillCard();
    
//     // Handle jail
//     if (currentPlayer->isInJail()) {
//         handleJailTurn();
//         return;
//     }
    
//     // Reset state
//     resetTurnState();
// }

// void GameManager::resetTurnState() {
//     consecutiveDoubles = 0;
//     if (currentContext) {
//         currentContext->setSkillUsed(false);
//         currentContext->setMovedByCard(false);
//         currentContext->resetDoubleCount();
//     }
// }

// void GameManager::rollDice() {
//     dice.roll();
//     std::cout << "Dadu: " << dice.getDie1() << " + " << dice.getDie2() 
//               << " = " << dice.getTotal() << std::endl;
    
//     if (dice.isDouble()) {
//         std::cout << "(Double!)" << std::endl;
//         consecutiveDoubles++;
        
//         if (consecutiveDoubles >= 3) {
//             std::cout << "3 kali double berturut-turut! Masuk penjara!" << std::endl;
//             getCurrentPlayer()->enterJail();
//             consecutiveDoubles = 0;
//             return;
//         }
//     } else {
//         consecutiveDoubles = 0;
//     }
    
//     // Move player
//     moveCurrentPlayer(dice.getTotal());
// }

// void GameManager::setManualDice(int d1, int d2) {
//     dice.setManual(d1, d2);
// }

// int GameManager::getLastDiceTotal() const {
//     return dice.getTotal();
// }

// bool GameManager::wasLastRollDouble() const {
//     return dice.isDouble();
// }

// void GameManager::moveCurrentPlayer(int steps) {
//     Player* player = getCurrentPlayer();
//     if (!player) return;
    
//     int oldPos = player->getPosition();
//     player->move(steps);
//     int newPos = player->getPosition();
    
//     // Check if passed GO
//     if (newPos < oldPos && steps > 0) {
//         // Passed GO, collect salary
//         player->addCash(config.special.goSalary);
//         std::cout << "Lewati GO! Dapat gaji M" << config.special.goSalary << std::endl;
//     }
    
//     // Get tile and handle effect
//     if (newPos >= 0 && newPos < static_cast<int>(tiles.size())) {
//         Tile* tile = tiles[newPos];
//         if (tile) {
//             std::cout << "Mendarat di: " << tile->getName() << " (" << tile->getCode() << ")" << std::endl;
//             tile->onLanded(*currentContext);
//         }
//     }
// }

// void GameManager::handleTileEffect(Tile* tile) {
//     // This will be handled by the tile's onLanded method
//     // We need to cast to specific tile types
//     // This is simplified - in reality, you'd use proper visitor pattern
//     // or dynamic casting
// }

// bool GameManager::nextTurn() {
//     // Check game end conditions
//     checkGameEnd();
    
//     if (status != GameStatus::RUNNING) {
//         return false;
//     }
    
//     // Handle double - same player plays again
//     if (consecutiveDoubles > 0 && consecutiveDoubles < 3 && !getCurrentPlayer()->isInJail()) {
//         std::cout << "Double! Giliran tambahan!" << std::endl;
//         return true;
//     }
    
//     // Move to next player
//     currentPlayerIndex = calculateNextPlayerIndex();
    
//     // If we've gone through all players, increment turn number
//     if (currentPlayerIndex == 0) {
//         currentTurnNumber++;
//     }
    
//     // Check max turn
//     if (config.misc.maxTurn > 0 && currentTurnNumber > config.misc.maxTurn) {
//         endGame();
//         return false;
//     }
    
//     return true;
// }

// int GameManager::calculateNextPlayerIndex() {
//     int nextIdx = (currentPlayerIndex + 1) % turnOrder.size();
    
//     // Skip bankrupt players
//     int attempts = 0;
//     while (attempts < turnOrder.size()) {
//         Player* player = players[turnOrder[nextIdx]];
//         if (player->getStatus() != PlayerStatus::BANKRUPT) {
//             break;
//         }
//         nextIdx = (nextIdx + 1) % turnOrder.size();
//         attempts++;
//     }
    
//     return nextIdx;
// }

// void GameManager::handleJailTurn() {
//     Player* player = getCurrentPlayer();
//     std::cout << "Anda sedang di penjara!" << std::endl;
//     std::cout << "Opsi:" << std::endl;
//     std::cout << "  1. Bayar denda M" << config.special.jailFine << std::endl;
//     std::cout << "  2. Gunakan kartu Bebas Penjara" << std::endl;
//     std::cout << "  3. Coba lempar dadu (double)" << std::endl;
//     // The actual choice will be handled by command handler
// }

// void GameManager::payJailFine() {
//     Player* player = getCurrentPlayer();
//     if (player->getBalance() >= config.special.jailFine) {
//         player->deductCash(config.special.jailFine);
//         player->exitJail();
//         std::cout << "Denda dibayar. Bebas dari penjara!" << std::endl;
//     } else {
//         std::cout << "Uang tidak cukup!" << std::endl;
//     }
// }

// void GameManager::useJailCard() {
//     // TODO: Check if player has jail card
//     // For now, just exit jail
//     getCurrentPlayer()->exitJail();
//     std::cout << "Kartu Bebas Penjara digunakan!" << std::endl;
// }

// void GameManager::tryJailRoll() {
//     dice.roll();
//     std::cout << "Dadu: " << dice.getDie1() << " + " << dice.getDie2() << std::endl;
    
//     if (dice.isDouble()) {
//         std::cout << "Double! Bebas dari penjara!" << std::endl;
//         getCurrentPlayer()->exitJail();
//         moveCurrentPlayer(dice.getTotal());
//     } else {
//         std::cout << "Bukan double. Masih di penjara." << std::endl;
//     }
// }

// void GameManager::drawSkillCard() {
//     if (skillDeck.empty()) {
//         // Reshuffle discard pile
//         if (skillDiscardPile.empty()) {
//             return;
//         }
//         skillDeck = skillDiscardPile;
//         skillDiscardPile.clear();
//         shuffleSkillDeck();
//     }
    
//     Player* player = getCurrentPlayer();
//     if (player->getHandsAmount() < 3) {
//         SkillCard* card = skillDeck.back();
//         skillDeck.pop_back();
//         player->drawSCard(card);
//     } else {
//         // Must discard first - handled elsewhere
//     }
// }

// void GameManager::checkGameEnd() {
//     // Count active players
//     int activeCount = 0;
//     Player* lastActive = nullptr;
    
//     for (Player* player : players) {
//         if (player->getStatus() != PlayerStatus::BANKRUPT) {
//             activeCount++;
//             lastActive = player;
//         }
//     }
    
//     // Bankruptcy mode (maxTurn < 1): only one player left
//     if (config.misc.maxTurn < 1 && activeCount <= 1) {
//         status = GameStatus::FINISHED;
//         if (lastActive) {
//             std::cout << "\n=== PEMENANG: " << lastActive->getUsername() << " ===" << std::endl;
//         }
//     }
    
//     // Max turn mode: check if we've reached max turn
//     if (config.misc.maxTurn > 0 && currentTurnNumber > config.misc.maxTurn) {
//         status = GameStatus::FINISHED;
//         declareWinner();
//     }
// }

// void GameManager::declareWinner() {
//     // Find player with most wealth
//     Player* winner = nullptr;
//     int maxWealth = -1;
    
//     for (Player* player : players) {
//         if (player->getStatus() == PlayerStatus::BANKRUPT) continue;
        
//         int wealth = player->getBalance();
//         // Add property values
//         for (Property* prop : player->getProperties()) {
//             wealth += prop->getBuyPrice();
//         }
        
//         if (wealth > maxWealth) {
//             maxWealth = wealth;
//             winner = player;
//         }
//     }
    
//     if (winner) {
//         std::cout << "\n=== PEMENANG: " << winner->getUsername() 
//                   << " dengan kekayaan M" << maxWealth << " ===" << std::endl;
//     }
// }

// void GameManager::endGame() {
//     status = GameStatus::FINISHED;
//     std::cout << "\n=== PERMAINAN SELESAI ===" << std::endl;
// }

// // Getters
// GameStatus GameManager::getStatus() const {
//     return status;
// }

// Player* GameManager::getCurrentPlayer() const {
//     if (turnOrder.empty() || currentPlayerIndex >= turnOrder.size()) {
//         return nullptr;
//     }
//     return players[turnOrder[currentPlayerIndex]];
// }

// Board* GameManager::getBoard() const {
//     return board;
// }

// int GameManager::getCurrentTurn() const {
//     return currentTurnNumber;
// }

// int GameManager::getMaxTurn() const {
//     return config.misc.maxTurn;
// }

// const std::vector<Player*>& GameManager::getAllPlayers() const {
//     return players;
// }

// TransactionLogger& GameManager::getLogger() {
//     return logger;
// }

// Config& GameManager::getConfig() {
//     return config;
// }

// // Placeholder implementations
// void GameManager::handlePropertyLanding(Property* prop) {}
// void GameManager::buyProperty(Property* prop) {}
// void GameManager::auctionProperty(Property* prop) {}
// void GameManager::mortgageProperty(Property* prop) {}
// void GameManager::redeemProperty(Property* prop) {}
// void GameManager::buildHouse(const std::string& propertyCode) {}
// void GameManager::sellHouse(const std::string& propertyCode) {}
// void GameManager::upgradeToHotel(const std::string& propertyCode) {}
// void GameManager::useSkillCard(int cardIndex) {}
// void GameManager::discardSkillCard(int cardIndex) {}
// void GameManager::handleBankruptcy(Player* player, int debtAmount, Player* creditor) {}

// void GameManager::saveGame(const std::string& filename) {
//     std::cout << "Menyimpan permainan ke: " << filename << "..." << std::endl;
    
//     // Build GameState from current game state
//     GameState state;
//     state.currentTurn = currentTurnNumber;
//     state.maxTurn = config.misc.maxTurn;
//     state.activePlayerIdx = currentPlayerIndex;
    
//     // Convert players to PlayerState
//     for (size_t i = 0; i < players.size(); ++i) {
//         Player* player = players[i];
//         PlayerState ps;
//         ps.username = player->getUsername();
//         ps.balance = player->getBalance();
//         ps.status = (player->getStatus() == PlayerStatus::ACTIVE) ? "ACTIVE" :
//                     (player->getStatus() == PlayerStatus::JAILED) ? "JAILED" : "BANKRUPT";
        
//         // Get position code from tile
//         int pos = player->getPosition();
//         if (pos >= 0 && pos < static_cast<int>(tiles.size())) {
//             ps.positionCode = tiles[pos]->getCode();
//         } else {
//             ps.positionCode = "GO";
//         }
        
//         // Convert skill cards to CardState
//         // Note: Player class needs method to access hand cards
//         // For now, we'll save empty hand
//         state.players.push_back(ps);
//     }
    
//     // Convert turn order indices
//     state.turnOrder = turnOrder;
    
//     // Convert properties to PropertyState
//     // This requires integration with actual Property objects
//     // For now, we'll save empty properties list
    
//     // Get deck state
//     for (SkillCard* card : skillDeck) {
//         state.deckState.push_back(card->getDescription());
//     }
    
//     // Get log entries
//     state.log = logger.getAll();
    
//     // Save using SaveLoadManager
//     SaveLoadManager saveManager;
//     saveManager.save(state, filename);
    
//     std::cout << "Permainan berhasil disimpan!" << std::endl;
// }

// void GameManager::loadGame(const std::string& filename) {
//     std::cout << "Memuat permainan dari: " << filename << "..." << std::endl;
    
//     // Check if file exists
//     std::ifstream test(filename);
//     if (!test.is_open()) {
//         std::cout << "File tidak ditemukan: " << filename << std::endl;
//         return;
//     }
//     test.close();
    
//     // Load using SaveLoadManager
//     SaveLoadManager loadManager;
//     GameState state = loadManager.load(filename);
    
//     // Restore game state
//     currentTurnNumber = state.currentTurn;
//     config.misc.maxTurn = state.maxTurn;
//     currentPlayerIndex = state.activePlayerIdx;
    
//     // Restore players
//     // Note: This would need proper cleanup of existing players and recreation
//     // For now, we'll just update existing players if counts match
//     if (state.players.size() != players.size()) {
//         std::cout << "Peringatan: Jumlah pemain tidak cocok dengan save file." << std::endl;
//     }
    
//     // Restore turn order
//     turnOrder = state.turnOrder;
    
//     // Restore log - serialize LogEntry vector to the expected format
//     // Format: turn\tusername\tactionType\tdetail\n
//     std::ostringstream logOss;
//     for (const auto& entry : state.log) {
//         logOss << entry.turn << "\t" << entry.username << "\t"
//                << entry.actionType << "\t" << entry.detail << "\n";
//     }
//     logger.deserialize(logOss.str());
    
//     std::cout << "Permainan berhasil dimuat!" << std::endl;
//     std::cout << "Giliran saat ini: " << currentTurnNumber << "/" << config.misc.maxTurn << std::endl;
//     if (getCurrentPlayer()) {
//         std::cout << "Pemain aktif: " << getCurrentPlayer()->getUsername() << std::endl;
//     }
// }
// void GameManager::printBoard() {
//     std::cout << "\n=== PAPAN NIMONSPOLI ===" << std::endl;
//     std::cout << "Posisi pemain:" << std::endl;
//     for (Player* player : players) {
//         if (player->getStatus() != PlayerStatus::BANKRUPT) {
//             int pos = player->getPosition();
//             if (pos >= 0 && pos < static_cast<int>(tiles.size())) {
//                 Tile* tile = tiles[pos];
//                 std::cout << "  " << player->getUsername() << " di " 
//                           << tile->getName() << " (" << tile->getCode() << ")" << std::endl;
//             }
//         }
//     }
    
//     std::cout << "\nDaftar petak:" << std::endl;
//     for (size_t i = 0; i < tiles.size(); ++i) {
//         Tile* tile = tiles[i];
//         std::cout << "  " << (i + 1) << ". [" << 
//                 (dynamic_cast<StreetProperty*>((dynamic_cast<PropertyTile*>(tile))->getProperty()) == nullptr ? "No Color" : dynamic_cast<StreetProperty*>((dynamic_cast<PropertyTile*>(tile))->getProperty())->getColor()) 
//                 << "] " 
//                   << tile->getCode() << " - " << tile->getName() << std::endl;
//     }
//     std::cout << "===" << std::endl;
// }

// void GameManager::printProperty(const std::string& code) {
//     for (Tile* tile : tiles) {
//         // Mencoba 'mengubah' Tile biasa menjadi PropertyTile
//         PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile); 
//         if (propTile != nullptr) {
//             if (propTile->getCode() == code) {
//                 std::cout << "\n=== AKTA KEPEMILIKAN ===" << std::endl;
//                 std::cout << "Nama: " << propTile->getName() << " (" << propTile->getCode() << ")" << std::endl;
//                 std::string cat;
//                 StreetProperty* strProp = dynamic_cast<StreetProperty*>(propTile->getProperty());
//                 if (strProp != nullptr){
//                     cat = "StreetProperty";
//                     std::cout << "Warna: " << strProp->getColorGroup() << std::endl;
//                 } else{
//                     if (dynamic_cast<RailroadTile*>(propTile) != nullptr){
//                         // adalah railroadTile 
//                         cat = "RailroadTile";
//                     } else{
//                         cat = "UtilityTile";
//                     }
//                 }
//                 std::cout << "Kategori: " << cat << std::endl;
//                 // TODO: Add more property details when Property is fully integrated
//                 std::cout << "==" << std::endl;
//                 return;
//             }
//         }
//     }
//     std::cout << "Properti dengan kode '" << code << "' tidak ditemukan." << std::endl;
// }

// void GameManager::printPlayerProperties() {
//     Player* player = getCurrentPlayer();
//     if (!player) {
//         std::cout << "Tidak ada pemain aktif!" << std::endl;
//         return;
//     }
    
//     std::cout << "\n=== PROPERTI MILIK " << player->getUsername() << " ===" << std::endl;
//     player->showProperties();
//     std::cout << "" << std::endl;
// }

// void GameManager::printLog(int count) {
//     std::cout << "\n=== LOG TRANSAKSI ===" << std::endl;
//     if (count > 0) {
//         std::cout << "Menampilkan " << count << " entri terakhir..." << std::endl;
//     } else {
//         std::cout << "Menampilkan semua log..." << std::endl;
//     }
//     // TODO: Implement actual log display when TransactionLogger is fully implemented
//     std::cout << "" << std::endl;
// }
// void GameManager::distributeSkillCards() {}
// void GameManager::movePlayerToTile(int tileIndex) {}
