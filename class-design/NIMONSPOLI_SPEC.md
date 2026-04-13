# NIMONSPOLI — Spesifikasi & Panduan Implementasi
> **Tugas Besar 1 · IF2010 Pemrograman Berorientasi Objek · Semester 2 2025/2026**  
> Kelompok **NUL — nullptr**  
> 13524031 Vincent Rionarlie · 13524033 Ray Owen Martin · 13524037 Nicholas Wise Saragih Sumbayak · 13524061 Muhammad Aufar Rizqi Kusuma · 13524065 Kurt Mikhael Purba

---

## Daftar Isi
1. [Gambaran Umum Proyek](#1-gambaran-umum-proyek)
2. [Arsitektur Layer](#2-arsitektur-layer)
3. [Diagram Kelas (PlantUML)](#3-diagram-kelas-plantuml)
4. [Deskripsi Setiap Kelas](#4-deskripsi-setiap-kelas)
5. [Alur Permainan](#5-alur-permainan-flow)
6. [Checklist Fitur](#6-checklist-fitur-yang-harus-diimplementasi)
7. [Panduan Implementasi untuk Model](#7-panduan-implementasi-untuk-model)
8. [Konvensi Kode](#8-konvensi-kode)

---

## 1. Gambaran Umum Proyek

**Nimonspoli** adalah implementasi permainan berbasis Monopoly dalam bahasa C++ dengan pendekatan **Object-Oriented Programming (OOP)**. Program berjalan via **CLI (Command-Line Interface)** dengan kemungkinan bonus GUI menggunakan Raylib.

**Aturan umum permainan:**
- Dimainkan oleh **2–4 pemain** (human dan/atau computer).
- Papan terdiri dari **20–60 petak** (default standard).
- Pemain melempar dua dadu setiap giliran dan bergerak di papan.
- Pemain dapat membeli properti, membangun rumah/hotel, membayar sewa, dan menggunakan kartu skill.
- Pemain yang bangkrut (tidak mampu bayar utang) dikeluarkan dari permainan.
- Permainan berakhir saat **maxTurn** tercapai atau hanya tersisa satu pemain. Pemenang ditentukan dari **total kekayaan tertinggi**.

---

## 2. Arsitektur Layer

```
┌─────────────────────────────────────────────┐
│               UI Layer                       │
│   CLIController · AuctionUI · BankruptcyUI  │
│   (Bonus: GUIController)                    │
└────────────────────┬────────────────────────┘
                     │ uses / notifies
┌────────────────────▼────────────────────────┐
│            Game Logic Layer                  │
│  GameEngine · TurnManager · AuctionManager  │
│  BankruptcyManager · Dice · TurnContext     │
│  Board · Player · Tile hierarchy            │
│  Property · Card · CardDeck                 │
└────────────────────┬────────────────────────┘
                     │ reads config via / persists via
┌────────────────────▼────────────────────────┐
│            Data Access Layer                 │
│  ConfigLoader · SaveLoadManager             │
│  TransactionLogger                          │
└─────────────────────────────────────────────┘
```

**Prinsip penting:**
- UI Layer **tidak** memodifikasi state secara langsung — hanya memanggil `GameEngine.executeCommand()`.
- Game Logic Layer **tidak** tahu cara membaca file — delegasikan ke Data Access Layer.
- Setiap kelas memiliki **Single Responsibility**.

---

## 3. Diagram Kelas (PlantUML)

File `.puml` tersedia di direktori proyek. Render menggunakan PlantUML atau ekstensi VS Code.

| File | Konten |
|------|--------|
| `02_tile_hierarchy.puml` | Hierarki Tile — Board, Tile abstract, semua subclass tile |
| `03_property_card.puml` | Hierarki Property (Street/Railroad/Utility) & Card (Chance/Community/Skill) |
| `04_player_game.puml` | Player, HumanPlayer, ComputerPlayer, Strategy, GameEngine, TurnManager, Dice |
| `05_io_saveload.puml` | ConfigLoader, Config, SaveLoadManager, GameState, TransactionLogger |
| `06_bonus_gui_com_dynamic.puml` | GUIController, BoardRenderer, Strategy pattern detail, BoardFactory, DynamicBoardLoader |

> **Catatan untuk model:** Selalu jadikan file `.puml` sebagai **sumber kebenaran** untuk nama atribut, method signature, dan relasi antar kelas. Jangan mengarang nama method yang tidak ada di diagram.

---

## 4. Deskripsi Setiap Kelas

### 4.1 UI Layer

#### `CLIController` *(abstract)*
- **Tanggung jawab:** Menangani semua I/O terminal. Menerima input teks dari pemain, meneruskan ke `GameEngine` via `executeCommand()`, dan mencetak output ke layar.
- **Method penting:** `run()`, `parseCommand(input)`, `printBoard()`, `printLog()`, `printDeed(code)`, `printLogin()`, `printProperty()`.
- **Relasi:** Menyimpan referensi ke `GameEngine`. Memberitahu `AuctionUI` dan `BankruptcyUI` saat event terjadi.

#### `AuctionUI` *(base)*
- **Tanggung jawab:** Menangani display dan input selama proses lelang properti.
- **Method penting:** `conductAuction(prop, players)` — loop penawaran, tampilkan harga tertinggi saat ini, minta keputusan setiap pemain (bid atau skip).
- **Relasi:** Berkomunikasi dengan `AuctionManager` untuk validasi bid.

#### `BankruptcyUI` *(base)*
- **Tanggung jawab:** Menampilkan panel likuidasi aset saat pemain tidak mampu bayar utang.
- **Method penting:** `showLiquidationPanel(player, debt)` — daftar properti yang bisa dijual/digadaikan beserta nilainya.
- **Relasi:** Berkoordinasi dengan `BankruptcyManager` untuk eksekusi aksi likuidasi.

---

### 4.2 Game Logic Layer

#### `GameEngine` *(base)*
- **Tanggung jawab:** Orkestrator utama. Menyimpan seluruh state program dan mendelegasikan eksekusi ke manajer yang sesuai.
- **Atribut krusial:** `board`, `players`, `turnMgr`, `logger`, `chanceDeck`, `communityDeck`, `skillDeck`, `config`.
- **Method penting:** `startNewGame()`, `loadGame(file)`, `saveGame(file)`, `executeCommand(cmd)`.

#### `TurnManager` *(base)*
- **Tanggung jawab:** Mengelola urutan giliran, melacak turn saat ini, mendeteksi akhir permainan, dan menentukan pemenang.
- **Atribut:** `currentTurn`, `maxTurn`, `activePlayerIdx`, `turnOrder: vector<int>`.
- **Method penting:** `nextTurn()`, `getCurrentPlayer()`, `isGameOver()`, `determineWinner()`.

#### `TurnContext` *(data class)*
- Objek konteks per giliran. Atribut: `currentPlayer`, `dice`, `board`, `engine`, `diceTotal`.

#### `Dice` *(base)*
- **Atribut:** `die1`, `die2`, `doubleCount`.
- **Method:** `roll()`, `setManual(x,y)`, `isDouble()`, `getTotal()`.
- **Aturan:** Triple double → pemain langsung masuk penjara.

#### `AuctionManager` *(base)*
- `runAuction(prop, players, startIdx) : Player*` — validasi bid, kembalikan pemenang lelang.

#### `BankruptcyManager` *(base)*
- `check(player, debt, creditor)` — cek kemampuan bayar.
- `liquidate(player, debt)` — minta pemain jual/gadai via BankruptcyUI.
- `declare(player, creditor)` — tandai bangkrut, transfer aset ke kreditor.

---

### 4.3 Tile Hierarchy

> Lihat `02_tile_hierarchy.puml` untuk diagram lengkap.

#### `Tile` *(abstract)*
- Atribut: `index`, `code`, `name`, `colorCategory`.
- Method abstract: `onLanded(player, ctx)` — dipanggil setiap pemain mendarat.
- Method lain: `getDisplay()`, `getColorTag()`, getters.

#### `PropertyTile` *(abstract, extends Tile)*
- Menyimpan pointer ke `Property`. Override `onLanded()`.

#### `StreetTile` *(extends PropertyTile)*
- `onLanded()` → beli / lelang / bayar sewa tergantung status properti.
- `triggerBuyOrAuction(player)`, `triggerRentPayment(player)`.

#### `RailroadTile` *(extends PropertyTile)*
- `onLanded()` → `autoAcquire()` — akuisisi otomatis atau bayar sewa berdasarkan jumlah railroad dimiliki pemilik.

#### `UtilityTile` *(extends PropertyTile)*
- `onLanded()` → `autoAcquire()`. Sewa = `multiplierTable` × `diceTotal`.

#### `GoTile` *(extends ActionTile)*
- `onLanded()` / dilewati → `paySalary(player)` sesuai `goSalary` dari config.

#### `JailTile` *(extends ActionTile)*
- Atribut: `inmates`, `visitors`. Method: `addInmate()`, `removeInmate()`, `isInmate()`.

#### `GoToJailTile` *(extends ActionTile)*
- `onLanded()` → pindahkan pemain ke JailTile dan jadikan inmate.

#### `CardTile` *(extends ActionTile)*
- Atribut: `type: CardType` (CHANCE / COMMUNITY).
- `onLanded()` → tarik & eksekusi kartu dari deck yang sesuai.

#### `FestivalTile` *(extends ActionTile)*
- `onLanded()` → `applyFestival(player, prop)` — naikkan `festivalMult` di StreetProperty pemain.

#### `TaxTile` *(extends ActionTile)*
- Atribut: `taxType: TaxType` (PPH / PBM).
- `onLanded()` → `applyPPH(player)` atau `applyPBM(player)`.

#### `FreeParkingTile` *(extends ActionTile)*
- Tidak ada efek khusus (bisa dikustomisasi).

#### `Board`
- `tiles: vector<Tile*>`, `codeToIndex: map<string,int>`.
- `buildFromConfig(data)`, `getColorGroup(color)`, `getTile(index)`, `getTileByCode(code)`.

---

### 4.4 Property & Card

> Lihat `03_property_card.puml` untuk diagram lengkap.

#### `Property` *(abstract)*
- Atribut: `code`, `name`, `status: PropertyStatus`, `owner`, `buyPrice`, `mortgageValue`.
- Status enum: `BANK`, `OWNED`, `MORTGAGED`.
- Method abstract: `getRent(ctx) : int`.
- Method lain: `mortgage()`, `redeem()`, `sellToBank() : int`.

#### `StreetProperty` *(extends Property)*
- `buildingCount`, `isHotel`, `rentTable: vector<int>`, `festivalMult`, `festivalDur`.
- `getRent()` → `rentTable[buildingCount]` × festivalMult.
- `buildHouse()`, `upgradeToHotel()`, `demolish()`, `applyFestival()`.
- `isMonopolized() : bool`, `canBuild() : bool`.

#### `RailroadProperty` *(extends Property)*
- `getRent()` → lookup `RailroadConfig.rentTable[jumlah_railroad_dimiliki_owner]`.

#### `UtilityProperty` *(extends Property)*
- `getRent()` → `UtilityConfig.multiplierTable[jumlah_utility_dimiliki_owner]` × `diceTotal`.

#### `CardDeck<T>` *(generic)*
- `draw()` — jika kosong, `refillFromDiscard()` lalu `shuffle()`.

#### `SkillCard` subclass

| Kelas | Efek |
|-------|------|
| `MoveCard` | Pindah pemain sebanyak `steps` petak |
| `DiscountCard` | Diskon sewa selama `duration` giliran |
| `ShieldCard` | Lindungi pemain dari satu pembayaran sewa |
| `TeleportCard` | Teleportasi ke petak manapun |
| `LassoCard` | Tarik pemain lain ke petak sendiri |
| `DemolitionCard` | Hancurkan bangunan di properti target |

---

### 4.5 I/O, Save/Load & Logging

> Lihat `05_io_saveload.puml` untuk diagram lengkap.

#### `ConfigLoader`
- `loadAll(path) : Config` — baca semua file config, hasilkan objek `Config`.

#### `Config` & sub-config

| Kelas | Isi |
|-------|-----|
| `PropertyData` | id, code, name, type, color, buyPrice, mortgageValue, houseUpgrade, hotelUpgrade, rentTable |
| `RailroadConfig` | `rentTable: map<int,int>` |
| `UtilityConfig` | `multiplierTable: map<int,int>` |
| `TaxConfig` | pphFlat, pphPercent, pbmFlat |
| `SpecialConfig` | goSalary, jailFine |
| `MiscConfig` | maxTurn, startingBalance |

#### `SaveLoadManager`
- `save(state, filename)` — serialisasi ke file teks.
- `load(filename) : GameState` — rekonstruksi state dari file.

#### `TransactionLogger`
- `log(turn, user, action, detail)` — catat setiap event penting.
- `getAll()`, `getLast(n)`, `serialize()`, `deserialize()`.

---

### 4.6 Bonus: GUI, COM, Dynamic Board

> Lihat `06_bonus_gui_com_dynamic.puml` untuk diagram lengkap.

#### GUI
- `GUIController` — implementasikan perintah yang sama dengan CLIController. **Hanya baca** state dari GameEngine.
- Library: **Raylib** (CMake FetchContent).
- `BoardRenderer::draw(window)`, `PlayerInfoPanel::draw(window)`.

#### COM — Strategy Pattern
- `ComputerPlayer` extends `Player`, override `decideAction()`.
- `AggressiveStrategy` — beli semua, bid tinggi, kartu ofensif.
- `ConservativeStrategy` — beli murah saja, bid rendah, tumpuk kartu defensif.
- Strategy di-inject saat konstruksi → mudah ditambah strategy baru.

#### Dynamic Board
- `DynamicBoardLoader::loadBoardConfig(file)` — baca layout papan dari file.
- Validasi: tepat 1 GoTile & 1 JailTile, 20 ≤ size ≤ 60, semua property code valid.
- `BoardFactory::createDefaultBoard(config)` dan `createDynamicBoard(tileConfigs)`.

---

## 5. Alur Permainan (Flow)

```
1. CLIController::run()
2. ConfigLoader::loadAll() → Config
3. Setup pemain
4. GameEngine::startNewGame()
5. LOOP GILIRAN:
   a. TurnManager::getCurrentPlayer()
   b. Player::decideAction(ctx)
      ├─ Dice::roll()
      ├─ Cek triple double → enterJail()
      ├─ Player::move(diceTotal)
      ├─ Cek lewat GoTile → paySalary()
      └─ Tile::onLanded(player, ctx)
           ├─ StreetTile → beli/lelang/sewa
           ├─ CardTile → draw & execute card
           ├─ GoToJailTile → enterJail()
           ├─ TaxTile → applyPPH/PBM
           └─ dll.
   c. BankruptcyManager::check() jika ada utang
   d. Pemain boleh gunakan SkillCard (maks 1/giliran)
   e. Pemain boleh bangun rumah/hotel
   f. TransactionLogger::log() setiap event
   g. TurnManager::nextTurn()
   h. Cek TurnManager::isGameOver()
6. TurnManager::determineWinner()
7. Tawarkan save / exit
```

---

## 6. Checklist Fitur yang Harus Diimplementasi

### Wajib (Core)
- [ ] Papan permainan dengan semua jenis tile
- [ ] Pergerakan pemain dengan dadu (termasuk triple double → penjara)
- [ ] Beli properti (Street, Railroad, Utility)
- [ ] Bayar sewa ke pemilik
- [ ] Sistem lelang properti (AuctionManager + AuctionUI)
- [ ] Sistem penjara (GoToJailTile, JailTile)
- [ ] Chance Card & Community Card
- [ ] Skill Card (6 subclass)
- [ ] Bangun rumah & hotel
- [ ] Festival Tile
- [ ] Pajak PPH & PBM
- [ ] Sistem kebangkrutan (BankruptcyManager + BankruptcyUI)
- [ ] Batas giliran & penentuan pemenang
- [ ] Save & Load (SaveLoadManager)
- [ ] Konfigurasi dari file (ConfigLoader)
- [ ] Log transaksi (TransactionLogger)
- [ ] CLI display (board, log, deed)

### Bonus
- [ ] GUI dengan Raylib (GUIController)
- [ ] ComputerPlayer + AggressiveStrategy & ConservativeStrategy
- [ ] Dynamic Board (BoardFactory + DynamicBoardLoader)

---

## 7. Panduan Implementasi untuk Model

> Instruksi khusus bagi AI model yang membantu mengerjakan proyek ini.

### Prinsip Utama
1. **File `.puml` adalah sumber kebenaran.** Nama class, atribut, method signature, dan relasi harus sesuai. Jangan menginventasi nama yang tidak ada di diagram.
2. **Bahasa: C++17.** Gunakan `std::vector`, `std::map`, `std::pair`, raw pointer sesuai diagram.
3. **Satu header + implementasi per kelas** (`ClassName.h` + `ClassName.cpp`).
4. **UI Layer tidak boleh akses Board/Player langsung** — hanya via `GameEngine::executeCommand()`.

### Struktur Direktori yang Disarankan
```
nimonspoli/
├── src/
│   ├── ui/           # CLIController, AuctionUI, BankruptcyUI
│   ├── engine/       # GameEngine, TurnManager, Dice, TurnContext
│   ├── tile/         # Tile abstract + semua subclass
│   ├── property/     # Property abstract + subclass
│   ├── card/         # Card hierarchy + CardDeck
│   ├── player/       # Player, HumanPlayer, ComputerPlayer, Strategy
│   ├── manager/      # AuctionManager, BankruptcyManager
│   ├── io/           # ConfigLoader, SaveLoadManager, TransactionLogger
│   └── bonus/        # GUIController, BoardFactory, DynamicBoardLoader
├── config/           # File konfigurasi (.txt)
├── saves/            # File save game
└── CMakeLists.txt
```

### Urutan Implementasi yang Disarankan
1. Data class & enum (`PropertyStatus`, `PlayerStatus`, `TurnContext`, `LogEntry`).
2. `ConfigLoader` + `Config` dan seluruh sub-config.
3. `Property` (abstract) → `StreetProperty`, `RailroadProperty`, `UtilityProperty`.
4. `Tile` (abstract) → `PropertyTile` → `StreetTile`, `RailroadTile`, `UtilityTile`.
5. `ActionTile` dan semua subclass action (GoTile, JailTile, dll.).
6. `Board`.
7. `Player` (abstract) → `HumanPlayer`.
8. `Dice`, `TurnManager`.
9. `Card` hierarchy + `CardDeck`.
10. `GameEngine` — wiring semua komponen.
11. `CLIController` — UI dan parsing perintah.
12. `AuctionManager` + `AuctionUI`.
13. `BankruptcyManager` + `BankruptcyUI`.
14. `TransactionLogger`.
15. `SaveLoadManager` + `GameState`.
16. **Bonus:** `ComputerPlayer` + Strategy, `GUIController`, `BoardFactory`.

### Kesalahan Umum yang Harus Dihindari
- ❌ Logika kalkulasi sewa di `Tile`, bukan di `Property`.
- ❌ Lupa `refillFromDiscard()` saat `CardDeck` kosong.
- ❌ Lupa reset `doubleCount` di `Dice` saat giliran berganti.
- ❌ `canBuild()` tanpa cek `isMonopolized()` terlebih dahulu.
- ❌ Tidak mengeluarkan pemain `BANKRUPT` dari `turnOrder` di `TurnManager`.
- ❌ Mengakses `Board` atau `Player` dari `CLIController` secara langsung.

### Method Virtual Penting (wajib dioverride)
```cpp
// Tile
virtual void onLanded(Player* player, TurnContext& ctx) = 0;

// Property
virtual int getRent(TurnContext& ctx) = 0;

// Player
virtual void decideAction(TurnContext& ctx) = 0;

// Card
virtual void execute(Player* player, TurnContext& ctx) = 0;

// SkillCard
virtual void use(Player* player, TurnContext& ctx) = 0;

// Strategy
virtual bool decideBuy(Property* prop, Player* player) = 0;
virtual int decideBid(int bid, Player* player) = 0;
virtual int decideSkill(vector<SkillCard*> hand, Player* player) = 0;
virtual Property* decideMortgage(vector<Property*> props) = 0;
```

---

## 8. Konvensi Kode

- **Naming:** `camelCase` untuk method & variabel, `PascalCase` untuk kelas, `UPPER_SNAKE_CASE` untuk enum.
- **Pointer:** Raw pointer (`Player*`) sesuai diagram. Gunakan smart pointer jika ownership eksplisit.
- **Const correctness:** Semua getter harus `const`.
- **Header guard:** Gunakan `#pragma once`.
- **Komentar:** Minimal satu baris per method non-trivial.
- **Include:** Minimal — gunakan forward declaration di header jika bisa.

---

*Dokumen ini di-generate dari Laporan M1 TB1 NUL-nullptr dan file diagram `.puml`. Selalu update dokumen ini jika ada perubahan desain kelas.*
