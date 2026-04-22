#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "exception/InvalidGameStateException.hpp"
#include "property/Property.hpp"
#include "property/RailroadProperty.hpp"
#include "property/StreetProperty.hpp"
#include "property/UtilityProperty.hpp"

class TurnContext {};

namespace {

class TestFailure : public std::runtime_error {
  public:
    explicit TestFailure(const std::string& message)
        : std::runtime_error(message) {}
};

class DummyProperty : public Property {
  public:
    DummyProperty(std::string code,
                  std::string name,
                  int buyPrice,
                  int mortgageValue,
                  int rent)
        : Property(std::move(code), std::move(name), buyPrice, mortgageValue),
          rent_(rent) {}

    int getRent(const TurnContext& ctx) const override {
        (void) ctx;
        return rent_;
    }

  private:
    int rent_;
};

struct TestCase {
    const char* name;
    void (*fn)();
};

void fail(const std::string& message) {
    throw TestFailure(message);
}

void expect_true(bool condition, const std::string& message) {
    if (!condition) {
        fail(message);
    }
}

template <typename Expected, typename Actual>
void expect_eq(const Expected& expected,
               const Actual& actual,
               const std::string& message) {
    if (!(expected == actual)) {
        fail(message + " | expected != actual");
    }
}

template <typename ExceptionType, typename Fn>
void expect_throw(Fn&& fn, const std::string& message) {
    try {
        fn();
    } catch (const ExceptionType&) {
        return;
    } catch (const std::exception& ex) {
        fail(message + " | wrong exception: " + ex.what());
    } catch (...) {
        fail(message + " | unknown exception type");
    }

    fail(message + " | expected exception was not thrown");
}

void test_property_accessors_and_state() {
    DummyProperty property("GRT", "Garut", 120, 60, 15);
    TurnContext ctx;

    expect_eq(std::string("GRT"), property.getCode(), "Property code");
    expect_eq(std::string("Garut"), property.getName(), "Property name");
    expect_eq(120, property.getBuyPrice(), "Property buy price");
    expect_eq(60, property.getMortgageValue(), "Property mortgage value");
    expect_eq(
        PropertyStatus::BANK, property.getStatus(), "Property default status");
    expect_eq(15, property.getRent(ctx), "Property rent");
    expect_true(property.getOwner() == nullptr,
                "Property owner should start null");

    property.setStatus(PropertyStatus::OWNED);
    expect_eq(
        PropertyStatus::OWNED, property.getStatus(), "Property setStatus");
}

void test_property_owner_and_lifecycle() {
    DummyProperty property("TSK", "Tasikmalaya", 140, 70, 20);

    property.setOwner(reinterpret_cast<Player*>(0x1));
    expect_true(property.getOwner() != nullptr, "Property setOwner");
    expect_eq(PropertyStatus::OWNED,
              property.getStatus(),
              "Property setOwner changes BANK to OWNED");

    property.mortgage();
    expect_eq(
        PropertyStatus::MORTGAGED, property.getStatus(), "Property mortgage");

    property.redeem();
    expect_eq(PropertyStatus::OWNED, property.getStatus(), "Property redeem");

    expect_eq(140, property.sellToBank(), "Property sellToBank value");
    expect_eq(PropertyStatus::BANK,
              property.getStatus(),
              "Property sellToBank resets status");
    expect_true(property.getOwner() == nullptr,
                "Property sellToBank resets owner");
}

void test_property_invalid_state_transitions() {
    DummyProperty property("BGR", "Bogor", 150, 75, 18);

    expect_throw<InvalidGameStateException>([&]() { property.mortgage(); },
                                            "Mortgage from BANK should fail");

    expect_throw<InvalidGameStateException>([&]() { property.redeem(); },
                                            "Redeem from BANK should fail");
}

void test_property_equality() {
    DummyProperty property_a("DPK", "Depok", 100, 50, 10);
    DummyProperty property_b("DPK", "Depok Copy", 999, 1, 999);
    DummyProperty property_c("BKS", "Bekasi", 100, 50, 10);

    expect_true(property_a == property_b, "Property equality by code");
    expect_true(!(property_a == property_c), "Property inequality by code");
}

void test_street_property_accessors_and_rent() {
    StreetProperty property(
        "GRT", "Garut", 100, 50, "COKLAT", 40, 100, {10, 20, 30, 40, 50, 60});
    TurnContext ctx;

    property.setOwner(reinterpret_cast<Player*>(0x1));

    expect_eq(
        std::string("COKLAT"), property.getColorGroup(), "Street color group");
    expect_eq(0, property.getBuildingCount(), "Street default building count");
    expect_true(!property.hasHotel(), "Street default hotel flag");
    expect_eq(40, property.getHousePrice(), "Street house price");
    expect_eq(100, property.getHotelPrice(), "Street hotel price");
    expect_eq(
        1, property.getFestivalMultiplier(), "Street festival multiplier");
    expect_eq(0, property.getFestivalDuration(), "Street festival duration");
    expect_true(!property.isMonopolized(), "Street monopoly default");
    expect_true(!property.canBuild(), "Street cannot build before monopoly");
    expect_eq(10, property.getRent(ctx), "Street base rent");

    property.setMonopolized(true);
    expect_true(property.canBuild(), "Street can build after monopoly");

    property.buildHouse();
    expect_eq(1, property.getBuildingCount(), "Street buildHouse increments");
    expect_eq(20, property.getRent(ctx), "Street rent increases with house");

    property.buildHouse();
    property.buildHouse();
    property.buildHouse();
    expect_eq(4, property.getBuildingCount(), "Street max houses before hotel");

    property.upgradeToHotel();
    expect_true(property.hasHotel(), "Street hotel flag after upgrade");
    expect_eq(
        5, property.getBuildingCount(), "Street building count after hotel");
    expect_eq(60, property.getRent(ctx), "Street rent at hotel level");
}

void test_street_property_festival_and_demolish() {
    StreetProperty property("TSK",
                            "Tasikmalaya",
                            100,
                            50,
                            "COKLAT",
                            40,
                            100,
                            {10, 20, 30, 40, 50, 60});
    property.setOwner(reinterpret_cast<Player*>(0x1));
    property.setMonopolized(true);

    property.buildHouse();
    property.applyFestival();
    expect_eq(
        2, property.getFestivalMultiplier(), "Festival doubles multiplier");
    expect_eq(3, property.getFestivalDuration(), "Festival resets duration");

    property.decreaseFestivalDuration();
    expect_eq(2, property.getFestivalDuration(), "Festival duration decreases");

    property.decreaseFestivalDuration();
    property.decreaseFestivalDuration();
    expect_eq(
        0, property.getFestivalDuration(), "Festival duration reaches zero");
    expect_eq(1,
              property.getFestivalMultiplier(),
              "Festival multiplier resets when duration ends");

    property.buildHouse();
    property.demolish();
    expect_eq(1, property.getBuildingCount(), "Street demolish removes house");
}

void test_street_property_invalid_rules() {
    expect_throw<InvalidGameStateException>(
        [&]() {
            StreetProperty property(
                "BAD", "Bad", 100, 50, "", 40, 100, {1, 2, 3, 4, 5, 6});
            (void) property;
        },
        "Street empty color group should fail");

    expect_throw<InvalidGameStateException>(
        [&]() {
            StreetProperty property(
                "BAD", "Bad", 100, 50, "RED", 40, 100, {1, 2, 3});
            (void) property;
        },
        "Street rent table shorter than L0-L5 should fail");
}

void test_railroad_property_accessors_and_rent() {
    RailroadProperty property("GBR",
                              "Stasiun Gambir",
                              200,
                              100,
                              {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    TurnContext ctx;

    property.setOwner(reinterpret_cast<Player*>(0x1));
    property.setOwnedRailroadCounter([](const Player*) { return 2; });

    expect_eq(
        50, property.getRent(ctx), "Railroad rent should match owned count");
    expect_eq(
        100, property.getRentFromOwnedCount(3), "Railroad rent by owned count");
}

void test_railroad_property_invalid_rules() {
    expect_throw<InvalidGameStateException>(
        [&]() {
            RailroadProperty property("GBR", "Stasiun Gambir", 200, 100, {});
            (void) property;
        },
        "Railroad empty rent table should fail");

    RailroadProperty property("GBR",
                              "Stasiun Gambir",
                              200,
                              100,
                              {{1, 25}, {2, 50}, {3, 100}, {4, 200}});
    expect_throw<InvalidGameStateException>(
        [&]() { property.getRentFromOwnedCount(0); },
        "Railroad owned count zero should fail");
}

void test_utility_property_accessors_and_rent() {
    UtilityProperty property("PLN", "PLN", 150, 75, {{1, 4}, {2, 10}});
    TurnContext ctx;

    property.setOwner(reinterpret_cast<Player*>(0x1));
    property.setOwnedUtilityCounter([](const Player*) { return 2; });
    property.setDiceTotalExtractor([](const TurnContext&) { return 7; });

    expect_eq(70,
              property.getRent(ctx),
              "Utility rent should use multiplier and dice total");
    expect_eq(28,
              property.getRentFromOwnedCountAndDice(1, 7),
              "Utility rent by owned count and dice");
}

void test_utility_property_invalid_rules() {
    expect_throw<InvalidGameStateException>(
        [&]() {
            UtilityProperty property("PLN", "PLN", 150, 75, {});
            (void) property;
        },
        "Utility empty multiplier table should fail");

    UtilityProperty property("PLN", "PLN", 150, 75, {{1, 4}, {2, 10}});
    expect_throw<InvalidGameStateException>(
        [&]() { property.getRentFromOwnedCountAndDice(0, 7); },
        "Utility owned count zero should fail");
    expect_throw<InvalidGameStateException>(
        [&]() { property.getRentFromOwnedCountAndDice(1, 0); },
        "Utility dice total zero should fail");
}

const std::vector<TestCase> kTests = {
    {"Property accessors and state", test_property_accessors_and_state},
    {"Property lifecycle", test_property_owner_and_lifecycle},
    {"Property invalid transitions", test_property_invalid_state_transitions},
    {"Property equality", test_property_equality},
    {"Street accessors and rent", test_street_property_accessors_and_rent},
    {"Street festival and demolish",
     test_street_property_festival_and_demolish},
    {"Street invalid rules", test_street_property_invalid_rules},
    {"Railroad accessors and rent", test_railroad_property_accessors_and_rent},
    {"Railroad invalid rules", test_railroad_property_invalid_rules},
    {"Utility accessors and rent", test_utility_property_accessors_and_rent},
    {"Utility invalid rules", test_utility_property_invalid_rules},
};

} // namespace

int main() {
    std::size_t passed = 0;
    std::size_t failed = 0;

    for (const TestCase& test_case : kTests) {
        try {
            test_case.fn();
            ++passed;
            std::cout << "[PASS] " << test_case.name << '\n';
        } catch (const TestFailure& failure) {
            ++failed;
            std::cout << "[FAIL] " << test_case.name << " - " << failure.what()
                      << '\n';
        } catch (const std::exception& ex) {
            ++failed;
            std::cout << "[FAIL] " << test_case.name
                      << " - unexpected error: " << ex.what() << '\n';
        } catch (...) {
            ++failed;
            std::cout << "[FAIL] " << test_case.name << " - unknown error"
                      << '\n';
        }
    }

    std::cout << "Summary: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
