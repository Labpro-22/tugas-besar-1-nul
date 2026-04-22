#include "core/GameManager.hpp"
#include "player/Player.h"
#include "property/Property.hpp"
#include "tile/Tile.hpp"
#include "tile/PropertyTile.hpp"
#include "tile/ActionTile.hpp"
#include "card/SkillCard.hpp"
#include "card/MoveCard.hpp"
#include "card/ShieldCard.hpp"
#include "card/DiscountCard.hpp"
#include "card/TeleportCard.hpp"
#include "card/LassoCard.hpp"
#include "card/DemolitionCard.hpp"
#include "core/BankruptcyManager.hpp"
#include "config/PropertyData.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

GameManager::GameManager() 
    : status(GameStatus::NOT_STARTED),
      board(nullptr),
      currentTurnNumber(1),
      currentPlayerIndex(0),
      currentContext(nullptr),
      consecutiveDoubles(0) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

GameManager::~GameManager() {
    // Clean up players
    for (Player* player : players) {
        delete player;
    }
    
    // Clean up tiles
    for (Tile* tile : tiles) {
        delete tile;
    }
    
    // Clean up board
    delete board;
    
    // Clean up skill cards
    for (SkillCard* card : skillDeck) {
        delete card;
    }
    for (SkillCard* card : skillDiscardPile) {
        delete card;
    }
    
    delete currentContext;
}

void GameManager::initialize(Config cfg) {
    config = cfg;
    initializeBoard();
    initializeSkillDeck();
}

void GameManager::initializeBoard() {
    // Create board with size from config or default 40
    board = new Board(40);
    
    // Build all 40 tiles according to the specification
    buildTiles();
}

void GameManager::buildTiles() {
    // Build all 40 tiles according to the specification
    // Index 0-39 (0-based) corresponds to position 1-40 (1-based in spec)
    
    // Position 1: GO (index 0)
    tiles.push_back(new GoTile(0, "GO", "Petak Mulai", "DF"));
    
    // Position 2: GRT - Garut (Street - Coklat)
    tiles.push_back(new StreetTile(1, "GRT", "Garut", "CK", nullptr));
    
    // Position 3: DNU - Dana Umum
    tiles.push_back(new CardTile(2, "DNU", "Dana Umum", "DF", false));
    
    // Position 4: TSK - Tasikmalaya (Street - Coklat)
    tiles.push_back(new StreetTile(3, "TSK", "Tasikmalaya", "CK", nullptr));
    
    // Position 5: PPH - Pajak Penghasilan
    tiles.push_back(new TaxTile(4, "PPH", "Pajak Penghasilan", "DF", TaxType::PPH));
    
    // Position 6: GBR - Stasiun Gambir (Railroad)
    tiles.push_back(new RailroadTile(5, "GBR", "Stasiun Gambir", "DF", nullptr));
    
    // Position 7: BGR - Bogor (Street - Biru Muda)
    tiles.push_back(new StreetTile(6, "BGR", "Bogor", "BM", nullptr));
    
    // Position 8: FES - Festival
    tiles.push_back(new FestivalTile(7, "FES", "Festival", "DF"));
    
    // Position 9: DPK - Depok (Street - Biru Muda)
    tiles.push_back(new StreetTile(8, "DPK", "Depok", "BM", nullptr));
    
    // Position 10: BKS - Bekasi (Street - Biru Muda)
    tiles.push_back(new StreetTile(9, "BKS", "Bekasi", "BM", nullptr));
    
    // Position 11: PEN - Penjara
    tiles.push_back(new JailTile(10, "PEN", "Penjara", "DF"));
    
    // Position 12: MGL - Magelang (Street - Pink)
    tiles.push_back(new StreetTile(11, "MGL", "Magelang", "PK", nullptr));
    
    // Position 13: PLN - PLN (Utility)
    tiles.push_back(new UtilityTile(12, "PLN", "PLN", "AB", nullptr));
    
    // Position 14: SOL - Solo (Street - Pink)
    tiles.push_back(new StreetTile(13, "SOL", "Solo", "PK", nullptr));
    
    // Position 15: YOG - Yogyakarta (Street - Pink)
    tiles.push_back(new StreetTile(14, "YOG", "Yogyakarta", "PK", nullptr));
    
    // Position 16: STB - Stasiun Bandung (Railroad)
    tiles.push_back(new RailroadTile(15, "STB", "Stasiun Bandung", "DF", nullptr));
    
    // Position 17: MAL - Malang (Street - Orange)
    tiles.push_back(new StreetTile(16, "MAL", "Malang", "OR", nullptr));
    
    // Position 18: DNU - Dana Umum
    tiles.push_back(new CardTile(17, "DNU", "Dana Umum", "DF", false));
    
    // Position 19: SMG - Semarang (Street - Orange)
    tiles.push_back(new StreetTile(18, "SMG", "Semarang", "OR", nullptr));
    
    // Position 20: SBY - Surabaya (Street - Orange)
    tiles.push_back(new StreetTile(19, "SBY", "Surabaya", "OR", nullptr));
    
    // Position 21: BBP - Bebas Parkir
    tiles.push_back(new FreeParkingTile(20, "BBP", "Bebas Parkir", "DF"));
    
    // Position 22: MKS - Makassar (Street - Merah)
    tiles.push_back(new StreetTile(21, "MKS", "Makassar", "MR", nullptr));
    
    // Position 23: KSP - Kesempatan
    tiles.push_back(new CardTile(22, "KSP", "Kesempatan", "DF", true));
    
    // Position 24: BLP - Balikpapan (Street - Merah)
    tiles.push_back(new StreetTile(23, "BLP", "Balikpapan", "MR", nullptr));
    
    // Position 25: MND - Manado (Street - Merah)
    tiles.push_back(new StreetTile(24, "MND", "Manado", "MR", nullptr));
    
    // Position 26: TUG - Stasiun Tugu (Railroad)
    tiles.push_back(new RailroadTile(25, "TUG", "Stasiun Tugu", "DF", nullptr));
    
    // Position 27: PLB - Palembang (Street - Kuning)
    tiles.push_back(new StreetTile(26, "PLB", "Palembang", "KN", nullptr));
    
    // Position 28: PKB - Pekanbaru (Street - Kuning)
    tiles.push_back(new StreetTile(27, "PKB", "Pekanbaru", "KN", nullptr));
    
    // Position 29: PAM - PAM (Utility)
    tiles.push_back(new UtilityTile(28, "PAM", "PAM", "AB", nullptr));
    
    // Position 30: MED - Medan (Street - Kuning)
    tiles.push_back(new StreetTile(29, "MED", "Medan", "KN", nullptr));
    
    // Position 31: PPJ - Pergi ke Penjara
    tiles.push_back(new GoToJailTile(30, "PPJ", "Pergi ke Penjara", "DF"));
    
    // Position 32: BDG - Bandung (Street - Hijau)
    tiles.push_back(new StreetTile(31, "BDG", "Bandung", "HJ", nullptr));
    
    // Position 33: DEN - Denpasar (Street - Hijau)
    tiles.push_back(new StreetTile(32, "DEN", "Denpasar", "HJ", nullptr));
    
    // Position 34: FES - Festival
    tiles.push_back(new FestivalTile(33, "FES", "Festival", "DF"));
    
    // Position 35: MTR - Mataram (Street - Hijau)
    tiles.push_back(new StreetTile(34, "MTR", "Mataram", "HJ", nullptr));
    
    // Position 36: GUB - Stasiun Gubeng (Railroad)
    tiles.push_back(new RailroadTile(35, "GUB", "Stasiun Gubeng", "DF", nullptr));
    
    // Position 37: KSP - Kesempatan
    tiles.push_back(new CardTile(36, "KSP", "Kesempatan", "DF", true));
    
    // Position 38: JKT - Jakarta (Street - Biru Tua)
    tiles.push_back(new StreetTile(37, "JKT", "Jakarta", "BT", nullptr));
    
    // Position 39: PBM - Pajak Barang Mewah
    tiles.push_back(new TaxTile(38, "PBM", "Pajak Barang Mewah", "DF", TaxType::PBM));
    
    // Position 40: IKN - Ibu Kota Nusantara (Street - Biru Tua)
    tiles.push_back(new StreetTile(39, "IKN", "Ibu Kota Nusantara", "BT", nullptr));
}

void GameManager::initializeSkillDeck() {
    // Create skill cards according to spec:
    // 4 MoveCards, 3 DiscountCards, 2 ShieldCards, 2 TeleportCards, 2 LassoCards, 2 DemolitionCards
    
    // Random number generator for card parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> moveDist(1, 10);
    std::uniform_int_distribution<> discountDist(10, 50);
    std::uniform_int_distribution<> teleportDist(0, 39);
    
    // MoveCards (4) - random steps 1-10
    for (int i = 0; i < 4; ++i) {
        skillDeck.push_back(new MoveCard(moveDist(gen)));
    }
    
    // DiscountCards (3) - random discount 10-50%
    for (int i = 0; i < 3; ++i) {
        skillDeck.push_back(new DiscountCard(discountDist(gen)));
    }
    
    // ShieldCards (2)
    for (int i = 0; i < 2; ++i) {
        skillDeck.push_back(new ShieldCard());
    }
    
    // TeleportCards (2) - random position
    for (int i = 0; i < 2; ++i) {
        skillDeck.push_back(new TeleportCard(teleportDist(gen)));
    }
    
    // LassoCards (2)
    for (int i = 0; i < 2; ++i) {
        skillDeck.push_back(new LassoCard());
    }
    
    // DemolitionCards (2)
    for (int i = 0; i < 2; ++i) {
        skillDeck.push_back(new DemolitionCard());
    }
    
    shuffleSkillDeck();
}

void GameManager::shuffleSkillDeck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(skillDeck.begin(), skillDeck.end(), g);
}

void GameManager::setupNewGame(int numPlayers, const std::vector<std::string>& usernames) {
    // Create players
    for (int i = 0; i < numPlayers; ++i) {
        Player* player = new Player(usernames[i], config.misc.startingBalance);
        player->setBoardSizeSource(&board->getSizeRef());
        players.push_back(player);
    }
    
    // Randomize turn order
    turnOrder.clear();
    for (int i = 0; i < numPlayers; ++i) {
        turnOrder.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(turnOrder.begin(), turnOrder.end(), g);
    
    currentPlayerIndex = 0;
    currentTurnNumber = 1;
}

void GameManager::setupLoadedGame(const std::vector<std::string>& usernames,
                                  const std::vector<int>& balances,
                                  const std::vector<int>& positions,
                                  const std::vector<std::string>& statuses,
                                  int currentTurn, int currentPlayerIdx) {
    int numPlayers = usernames.size();
    
    for (int i = 0; i < numPlayers; ++i) {
        Player* player = new Player(usernames[i], balances[i]);
        player->setBoardSizeSource(&board->getSizeRef());
        
        // Set position
        for (int j = 0; j < positions[i]; ++j) {
            player->move(1);
        }
        
        // Set status
        if (statuses[i] == "JAILED") {
            player->enterJail();
        } else if (statuses[i] == "BANKRUPT") {
            player->setBankruptStatus();
        }
        
        players.push_back(player);
        turnOrder.push_back(i);
    }
    
    currentTurnNumber = currentTurn;
    currentPlayerIndex = currentPlayerIdx;
}

void GameManager::startGame() {
    status = GameStatus::RUNNING;
    std::cout << "=== PERMAINAN DIMULAI ===" << std::endl;
    std::cout << "Jumlah pemain: " << players.size() << std::endl;
    std::cout << "Urutan giliran:" << std::endl;
    for (size_t i = 0; i < turnOrder.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << players[turnOrder[i]]->getUsername() << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

void GameManager::runGameLoop() {
    while (status == GameStatus::RUNNING) {
        processTurn();
        
        if (!nextTurn()) {
            break;
        }
    }
}

void GameManager::processTurn() {
    Player* currentPlayer = getCurrentPlayer();
    if (currentPlayer == nullptr || currentPlayer->getStatus() == PlayerStatus::BANKRUPT) {
        return;
    }
    
    std::cout << "\n========== TURN " << currentTurnNumber << " / " << config.misc.maxTurn 
              << " ==========" << std::endl;
    std::cout << "Giliran: " << currentPlayer->getUsername() << std::endl;
    std::cout << "Posisi: " << currentPlayer->getPosition() << std::endl;
    std::cout << "Uang: M" << currentPlayer->getBalance() << std::endl;
    
    // Create turn context
    delete currentContext;
    currentContext = new TurnContext(currentPlayer, board, currentTurnNumber, 
                                     config.misc.maxTurn, players);
    currentContext->setLogger(&logger);
    
    // Draw skill card at start of turn
    drawSkillCard();
    
    // Handle jail
    if (currentPlayer->isInJail()) {
        handleJailTurn();
        return;
    }
    
    // Reset state
    resetTurnState();
}

void GameManager::resetTurnState() {
    consecutiveDoubles = 0;
    if (currentContext) {
        currentContext->setSkillUsed(false);
        currentContext->setMovedByCard(false);
        currentContext->resetDoubleCount();
    }
}

void GameManager::rollDice() {
    dice.roll();
    std::cout << "Dadu: " << dice.getDie1() << " + " << dice.getDie2() 
              << " = " << dice.getTotal() << std::endl;
    
    if (dice.isDouble()) {
        std::cout << "(Double!)" << std::endl;
        consecutiveDoubles++;
        
        if (consecutiveDoubles >= 3) {
            std::cout << "3 kali double berturut-turut! Masuk penjara!" << std::endl;
            getCurrentPlayer()->enterJail();
            consecutiveDoubles = 0;
            return;
        }
    } else {
        consecutiveDoubles = 0;
    }
    
    // Move player
    moveCurrentPlayer(dice.getTotal());
}

void GameManager::setManualDice(int d1, int d2) {
    dice.setManual(d1, d2);
}

int GameManager::getLastDiceTotal() const {
    return dice.getTotal();
}

bool GameManager::wasLastRollDouble() const {
    return dice.isDouble();
}

void GameManager::moveCurrentPlayer(int steps) {
    Player* player = getCurrentPlayer();
    if (!player) return;
    
    int oldPos = player->getPosition();
    player->move(steps);
    int newPos = player->getPosition();
    
    // Check if passed GO
    if (newPos < oldPos && steps > 0) {
        // Passed GO, collect salary
        player->addCash(config.special.goSalary);
        std::cout << "Lewati GO! Dapat gaji M" << config.special.goSalary << std::endl;
    }
    
    // Get tile and handle effect
    if (newPos >= 0 && newPos < static_cast<int>(tiles.size())) {
        Tile* tile = tiles[newPos];
        if (tile) {
            std::cout << "Mendarat di: " << tile->getName() << " (" << tile->getCode() << ")" << std::endl;
            tile->onLanded(player, *currentContext);
        }
    }
}

void GameManager::handleTileEffect(Tile* tile) {
    // This will be handled by the tile's onLanded method
    // We need to cast to specific tile types
    // This is simplified - in reality, you'd use proper visitor pattern
    // or dynamic casting
}

bool GameManager::nextTurn() {
    // Check game end conditions
    checkGameEnd();
    
    if (status != GameStatus::RUNNING) {
        return false;
    }
    
    // Handle double - same player plays again
    if (consecutiveDoubles > 0 && consecutiveDoubles < 3 && !getCurrentPlayer()->isInJail()) {
        std::cout << "Double! Giliran tambahan!" << std::endl;
        return true;
    }
    
    // Move to next player
    currentPlayerIndex = calculateNextPlayerIndex();
    
    // If we've gone through all players, increment turn number
    if (currentPlayerIndex == 0) {
        currentTurnNumber++;
    }
    
    // Check max turn
    if (config.misc.maxTurn > 0 && currentTurnNumber > config.misc.maxTurn) {
        endGame();
        return false;
    }
    
    return true;
}

int GameManager::calculateNextPlayerIndex() {
    int nextIdx = (currentPlayerIndex + 1) % turnOrder.size();
    
    // Skip bankrupt players
    int attempts = 0;
    while (attempts < turnOrder.size()) {
        Player* player = players[turnOrder[nextIdx]];
        if (player->getStatus() != PlayerStatus::BANKRUPT) {
            break;
        }
        nextIdx = (nextIdx + 1) % turnOrder.size();
        attempts++;
    }
    
    return nextIdx;
}

void GameManager::handleJailTurn() {
    Player* player = getCurrentPlayer();
    std::cout << "Anda sedang di penjara!" << std::endl;
    std::cout << "Opsi:" << std::endl;
    std::cout << "  1. Bayar denda M" << config.special.jailFine << std::endl;
    std::cout << "  2. Gunakan kartu Bebas Penjara" << std::endl;
    std::cout << "  3. Coba lempar dadu (double)" << std::endl;
    // The actual choice will be handled by command handler
}

void GameManager::payJailFine() {
    Player* player = getCurrentPlayer();
    if (player->getBalance() >= config.special.jailFine) {
        player->deductCash(config.special.jailFine);
        player->exitJail();
        std::cout << "Denda dibayar. Bebas dari penjara!" << std::endl;
    } else {
        std::cout << "Uang tidak cukup!" << std::endl;
    }
}

void GameManager::useJailCard() {
    // TODO: Check if player has jail card
    // For now, just exit jail
    getCurrentPlayer()->exitJail();
    std::cout << "Kartu Bebas Penjara digunakan!" << std::endl;
}

void GameManager::tryJailRoll() {
    dice.roll();
    std::cout << "Dadu: " << dice.getDie1() << " + " << dice.getDie2() << std::endl;
    
    if (dice.isDouble()) {
        std::cout << "Double! Bebas dari penjara!" << std::endl;
        getCurrentPlayer()->exitJail();
        moveCurrentPlayer(dice.getTotal());
    } else {
        std::cout << "Bukan double. Masih di penjara." << std::endl;
    }
}

void GameManager::drawSkillCard() {
    if (skillDeck.empty()) {
        // Reshuffle discard pile
        if (skillDiscardPile.empty()) {
            return;
        }
        skillDeck = skillDiscardPile;
        skillDiscardPile.clear();
        shuffleSkillDeck();
    }
    
    Player* player = getCurrentPlayer();
    if (player->getHandsAmount() < 3) {
        SkillCard* card = skillDeck.back();
        skillDeck.pop_back();
        player->drawSCard(card);
    } else {
        // Must discard first - handled elsewhere
    }
}

void GameManager::checkGameEnd() {
    // Count active players
    int activeCount = 0;
    Player* lastActive = nullptr;
    
    for (Player* player : players) {
        if (player->getStatus() != PlayerStatus::BANKRUPT) {
            activeCount++;
            lastActive = player;
        }
    }
    
    // Bankruptcy mode (maxTurn < 1): only one player left
    if (config.misc.maxTurn < 1 && activeCount <= 1) {
        status = GameStatus::FINISHED;
        if (lastActive) {
            std::cout << "\n=== PEMENANG: " << lastActive->getUsername() << " ===" << std::endl;
        }
    }
    
    // Max turn mode: check if we've reached max turn
    if (config.misc.maxTurn > 0 && currentTurnNumber > config.misc.maxTurn) {
        status = GameStatus::FINISHED;
        declareWinner();
    }
}

void GameManager::declareWinner() {
    // Find player with most wealth
    Player* winner = nullptr;
    int maxWealth = -1;
    
    for (Player* player : players) {
        if (player->getStatus() == PlayerStatus::BANKRUPT) continue;
        
        int wealth = player->getBalance();
        // Add property values
        for (Property* prop : player->getProperties()) {
            wealth += prop->getBuyPrice();
        }
        
        if (wealth > maxWealth) {
            maxWealth = wealth;
            winner = player;
        }
    }
    
    if (winner) {
        std::cout << "\n=== PEMENANG: " << winner->getUsername() 
                  << " dengan kekayaan M" << maxWealth << " ===" << std::endl;
    }
}

void GameManager::endGame() {
    status = GameStatus::FINISHED;
    std::cout << "\n=== PERMAINAN SELESAI ===" << std::endl;
}

// Getters
GameStatus GameManager::getStatus() const {
    return status;
}

Player* GameManager::getCurrentPlayer() const {
    if (turnOrder.empty() || currentPlayerIndex >= turnOrder.size()) {
        return nullptr;
    }
    return players[turnOrder[currentPlayerIndex]];
}

Board* GameManager::getBoard() const {
    return board;
}

int GameManager::getCurrentTurn() const {
    return currentTurnNumber;
}

int GameManager::getMaxTurn() const {
    return config.misc.maxTurn;
}

const std::vector<Player*>& GameManager::getAllPlayers() const {
    return players;
}

TransactionLogger& GameManager::getLogger() {
    return logger;
}

Config& GameManager::getConfig() {
    return config;
}

// Placeholder implementations
void GameManager::handlePropertyLanding(Property* prop) {}
void GameManager::buyProperty(Property* prop) {}
void GameManager::auctionProperty(Property* prop) {}
void GameManager::mortgageProperty(Property* prop) {}
void GameManager::redeemProperty(Property* prop) {}
void GameManager::buildHouse(const std::string& propertyCode) {}
void GameManager::sellHouse(const std::string& propertyCode) {}
void GameManager::upgradeToHotel(const std::string& propertyCode) {}
void GameManager::useSkillCard(int cardIndex) {}
void GameManager::discardSkillCard(int cardIndex) {}
void GameManager::handleBankruptcy(Player* player, int debtAmount, Player* creditor) {}
void GameManager::saveGame(const std::string& filename) {}
void GameManager::loadGame(const std::string& filename) {}
void GameManager::printBoard() {
    std::cout << "\n========== PAPAN NIMONSPOLI ==========" << std::endl;
    std::cout << "Posisi pemain:" << std::endl;
    for (Player* player : players) {
        if (player->getStatus() != PlayerStatus::BANKRUPT) {
            int pos = player->getPosition();
            if (pos >= 0 && pos < static_cast<int>(tiles.size())) {
                Tile* tile = tiles[pos];
                std::cout << "  " << player->getUsername() << " di " 
                          << tile->getName() << " (" << tile->getCode() << ")" << std::endl;
            }
        }
    }
    
    std::cout << "\nDaftar petak:" << std::endl;
    for (size_t i = 0; i < tiles.size(); ++i) {
        Tile* tile = tiles[i];
        std::cout << "  " << (i + 1) << ". [" << tile->getColorTag() << "] " 
                  << tile->getCode() << " - " << tile->getName() << std::endl;
    }
    std::cout << "======================================" << std::endl;
}

void GameManager::printProperty(const std::string& code) {
    for (Tile* tile : tiles) {
        if (tile->getCode() == code) {
            std::cout << "\n========== AKTA KEPMILIKAN ==========" << std::endl;
            std::cout << "Nama: " << tile->getName() << " (" << tile->getCode() << ")" << std::endl;
            std::cout << "Kategori: " << tile->getColorTag() << std::endl;
            // TODO: Add more property details when Property is fully integrated
            std::cout << "=====================================" << std::endl;
            return;
        }
    }
    std::cout << "Properti dengan kode '" << code << "' tidak ditemukan." << std::endl;
}

void GameManager::printPlayerProperties() {
    Player* player = getCurrentPlayer();
    if (!player) {
        std::cout << "Tidak ada pemain aktif!" << std::endl;
        return;
    }
    
    std::cout << "\n========== PROPERTI MILIK " << player->getUsername() << " ==========" << std::endl;
    player->showProperties();
    std::cout << "==========================================" << std::endl;
}

void GameManager::printLog(int count) {
    std::cout << "\n========== LOG TRANSAKSI ==========" << std::endl;
    if (count > 0) {
        std::cout << "Menampilkan " << count << " entri terakhir..." << std::endl;
    } else {
        std::cout << "Menampilkan semua log..." << std::endl;
    }
    // TODO: Implement actual log display when TransactionLogger is fully implemented
    std::cout << "===================================" << std::endl;
}
void GameManager::distributeSkillCards() {}
void GameManager::movePlayerToTile(int tileIndex) {}
