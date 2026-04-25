#pragma once

#include <string>
#include <vector>
#include <memory>

#if __has_include(<raylib.h>)
#include <raylib.h>
#define NIMONSPOLI_HAS_RAYLIB 1
#else
#define NIMONSPOLI_HAS_RAYLIB 0
#endif

// Forward declarations
class Player;
class Property;
class TurnContext;
class Board;
class TransactionLogger;
class GameEngine;

namespace GuiPanels {

enum class PanelType {
    NONE,
    PROPERTY_BUY,
    AUCTION,
    BANKRUPTCY,
    UPGRADE,
    SKILL_CARD,
    MORTGAGE,
    DISMORTGAGE,
    SAVE,
    PROPERTY_CERT,
    PROFILE,
    PROPERTIES_LIST,
    LOG,
    MESSAGE,
    INPUT_DIALOG,
    FESTIVAL,
    TAX
};

enum class ActionResult {
    PENDING,
    DONE,
    CANCELLED
};

// ============================================================================
// Panel state structures
// ============================================================================

struct PropertyBuyState {
    Player* player = nullptr;
    Property* property = nullptr;
    TurnContext* ctx = nullptr;
    ActionResult result = ActionResult::PENDING;
};

struct AuctionState {
    Property* property = nullptr;
    std::vector<Player*> bidders;
    int startIdx = 0;
    int highestBid = -1;
    Player* winner = nullptr;
    int consecutivePasses = 0;
    int turnIdx = 0;
    int activeCount = 0;
    std::string statusMsg;
    ActionResult result = ActionResult::PENDING;
    std::string bidInput;
    bool inputActive = false;
    float botDelay = 0.0f;
    TurnContext* ctx = nullptr;
};

struct BankruptcyState {
    TurnContext* ctx = nullptr;
    int debtAmount = 0;
    ActionResult result = ActionResult::PENDING;
    struct Option {
        Property* prop = nullptr;
        int type = 0; // 0 = sell, 1 = mortgage
        int value = 0;
        std::string label;
    };
    std::vector<Option> options;
    int pendingRent = 0;
    Player* rentOwner = nullptr;
};

struct UpgradeState {
    Player* player = nullptr;
    TurnContext* ctx = nullptr;
    std::vector<Property*> eligible;
    ActionResult result = ActionResult::PENDING;
};

struct SkillCardState {
    Player* player = nullptr;
    TurnContext* ctx = nullptr;
    ActionResult result = ActionResult::PENDING;
    int selectedIndex = -1;
};

struct MortgageState {
    Player* player = nullptr;
    ActionResult result = ActionResult::PENDING;
    std::vector<Property*> eligible;
};

struct DismortgageState {
    Player* player = nullptr;
    ActionResult result = ActionResult::PENDING;
    std::vector<Property*> eligible;
};

struct SaveState {
    GameEngine* engine = nullptr;
    ActionResult result = ActionResult::PENDING;
    std::string filename;
    bool inputActive = false;
};

struct PropertyCertState {
    Board* board = nullptr;
    ActionResult result = ActionResult::PENDING;
    std::string propertyCode;
    bool inputActive = false;
};

struct MessageState {
    std::string title;
    std::string message;
    bool acknowledged = false;
};

struct ProfileState {
    Player* player = nullptr;
};

struct PropertiesListState {
    Player* player = nullptr;
};

struct LogState {
    TransactionLogger* logger = nullptr;
};

struct FestivalState {
    Player* player = nullptr;
    TurnContext* ctx = nullptr;
    std::vector<Property*> ownedProps;
    ActionResult result = ActionResult::PENDING;
};

struct TaxState {
    Player* player = nullptr;
    int taxFlat = 0;
    float taxPercent = 0.0f;
    ActionResult result = ActionResult::PENDING;
};

// ============================================================================
// Panel Manager
// ============================================================================

class PanelManager {
  public:
    PanelManager();

    // Query
    bool isActive() const;
    bool isBlocking() const;
    PanelType getType() const;

    // Lifecycle - call between BeginDrawing/EndDrawing
    void updateAndRender();

    // Clear current panel
    void dismiss();

    // -------------------------------------------------------------------------
    // Panel requests
    // -------------------------------------------------------------------------
    void showPropertyBuy(Player& player, Property& prop, TurnContext& ctx);
    void showAuction(Property& prop, const std::vector<Player*>& players,
                     int startIdx, TurnContext& ctx);
    void showBankruptcy(TurnContext& ctx, int debtAmount);
    void showUpgrade(Player& player, TurnContext& ctx);
    void showSkillCard(Player& player, TurnContext& ctx);
    void showMortgage(Player& player);
    void showDismortgage(Player& player);
    void showSave(GameEngine& engine);
    void showPropertyCert(Board& board);
    void showProfile(Player& player);
    void showPropertiesList(Player& player);
    void showLog(TransactionLogger& logger);
    void showMessage(const std::string& title, const std::string& message);
    void showFestival(Player& player, TurnContext& ctx);
    void showTax(Player& player, int taxFlat, float taxPercent);

    // -------------------------------------------------------------------------
    // Accessors for caller to read results / execute follow-up
    // -------------------------------------------------------------------------
    PropertyBuyState& getBuyState() { return buy_; }
    AuctionState& getAuctionState() { return auction_; }
    BankruptcyState& getBankruptcyState() { return bankruptcy_; }
    UpgradeState& getUpgradeState() { return upgrade_; }
    SkillCardState& getSkillState() { return skill_; }
    MortgageState& getMortgageState() { return mortgage_; }
    DismortgageState& getDismortgageState() { return dismortgage_; }
    SaveState& getSaveState() { return save_; }
    PropertyCertState& getCertState() { return cert_; }
    MessageState& getMessageState() { return message_; }
    ProfileState& getProfileState() { return profile_; }
    PropertiesListState& getPropertiesState() { return properties_; }
    LogState& getLogState() { return log_; }
    FestivalState& getFestivalState() { return festival_; }
    TaxState& getTaxState() { return tax_; }

  private:
    PanelType active_ = PanelType::NONE;
    bool blocking_ = false;

    PropertyBuyState buy_;
    AuctionState auction_;
    BankruptcyState bankruptcy_;
    UpgradeState upgrade_;
    SkillCardState skill_;
    MortgageState mortgage_;
    DismortgageState dismortgage_;
    SaveState save_;
    PropertyCertState cert_;
    MessageState message_;
    ProfileState profile_;
    PropertiesListState properties_;
    LogState log_;
    FestivalState festival_;
    TaxState tax_;

    // Text input helper
    struct TextInput {
        std::string text;
        bool active = false;
        float backspaceTimer = 0.0f;
        void update();
        void draw(float x, float y, float w, float h, const char* hint);
        bool checkClick(float x, float y, float w, float h);
    };
    TextInput textInput_;

    // Rendering
    void renderPropertyBuy();
    void renderAuction();
    void renderBankruptcy();
    void renderUpgrade();
    void renderSkillCard();
    void renderMortgage();
    void renderDismortgage();
    void renderSave();
    void renderPropertyCert();
    void renderProfile();
    void renderPropertiesList();
    void renderLog();
    void renderMessage();
    void renderFestival();
    void renderTax();

    // Helpers
    static bool button(const char* text, float x, float y, float w, float h,
                       Color bg, Color textColor, Color hover);
    static void centeredText(const char* text, int y, int fontSize, Color color);
    static void wrappedText(const std::string& text, int x, int y, int maxWidth,
                            int fontSize, Color color, int lineHeight);
    static void panelBg(int w, int h, const char* title);
    static bool isMouseOver(float x, float y, float w, float h);
};

} // namespace GuiPanels
