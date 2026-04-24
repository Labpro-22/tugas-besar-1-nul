#include "core/AuctionManager.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "exception/CommandException.hpp"
#include "exception/InsufficientFundsException.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"

namespace {

std::string toUpper(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(), [](unsigned char ch) {
            return static_cast<char>(std::toupper(ch));
        });
    return value;
}

bool tryParseBidLine(const std::string& line, bool& isPass, int& bidValue) {
    std::istringstream iss(line);
    std::string action;
    if (!(iss >> action)) {
        return false;
    }

    action = toUpper(action);
    if (action == "PASS") {
        std::string extra;
        if (iss >> extra) {
            return false;
        }
        isPass = true;
        bidValue = -1;
        return true;
    }

    if (action == "BID") {
        int amount = 0;
        if (!(iss >> amount)) {
            return false;
        }
        std::string extra;
        if (iss >> extra) {
            return false;
        }
        isPass = false;
        bidValue = amount;
        return true;
    }

    return false;
}

std::vector<Player*> buildAuctionOrder(const std::vector<Player*>& players,
                                       int startIdx) {
    std::vector<Player*> order;
    if (players.empty()) {
        return order;
    }

    const int n = static_cast<int>(players.size());
    int begin = 0;
    if (n > 0) {
        begin = ((startIdx % n) + n) % n;
    }

    for (int k = 0; k < n; ++k) {
        const int idx = (begin + k) % n;
        Player* player = players[static_cast<std::size_t>(idx)];
        if (player != nullptr &&
            player->getStatus() != PlayerStatus::BANKRUPT) {
            order.push_back(player);
        }
    }

    return order;
}

} // namespace


AuctionWinner::AuctionWinner(Player& player, Property& prop, int amount):
    winner(player), prop_won(prop), buyAmount(amount){};

AuctionWinner AuctionManager::runAuction(Property& prop,
                                   const std::vector<Player*>& players,
                                   int startIdx) {
    lastWinningBid_ = -1;
    // harusnya udah ga butuh, prop memang tidak boleh null karena reference
    // if (prop == nullptr) {
    //     throw CommandException("Auction property cannot be null");
    // }

    std::vector<Player*> order = buildAuctionOrder(players, startIdx + 1);
    if (order.empty()) {
        throw CommandException("Auction order cannot be empty");
    }

    const int activeCount = static_cast<int>(order.size());
    int highestBid = -1;
    Player* winner = nullptr;
    int consecutivePasses = 0;
    int turnIdx = 0;

    std::cout << "[LELANG] Memulai lelang untuk " << prop.getName() << " ("
              << prop.getCode() << ")\n";

    while (true) {
        Player* current = order[static_cast<std::size_t>(turnIdx)];
        if (current == nullptr) {
            throw CommandException("Encountered null player during auction");
        }

        while (true) {
            std::cout << "[LELANG] " << current->getUsername()
                      << " -> masukkan PASS atau BID <jumlah>: ";

            std::string input;
            std::getline(std::cin, input);

            bool isPass = false;
            int bidValue = -1;
            if (!tryParseBidLine(input, isPass, bidValue)) {
                std::cout << "[LELANG] Input tidak valid. Contoh: PASS atau "
                             "BID 250\n";
                continue;
            }

            if (isPass) {
                ++consecutivePasses;

                // Spec rule: if no bids at all and termination condition is
                // reached, the last player asked must place a minimum valid
                // bid.
                if (winner == nullptr && consecutivePasses >= activeCount - 1) {
                    highestBid = 0;
                    winner = current;
                    lastWinningBid_ = highestBid;
                    std::cout << "[LELANG] Tidak ada bid. "
                              << current->getUsername()
                              << " wajib bid minimum 0.\n";
                    return AuctionWinner(*winner, prop, 0);
                }

                if (winner != nullptr && consecutivePasses >= activeCount - 1) {
                    lastWinningBid_ = highestBid;
                    std::cout << "[LELANG] Pemenang: " << winner->getUsername()
                              << " dengan bid " << highestBid << "\n";
                    return AuctionWinner(*winner, prop, highestBid);
                }

                break;
            }

            if (bidValue < 0) {
                std::cout << "[LELANG] Bid harus >= 0.\n";
                continue;
            }

            if (winner != nullptr && bidValue <= highestBid) {
                std::cout << "[LELANG] Bid harus lebih tinggi dari "
                          << highestBid << ".\n";
                continue;
            }

            if (bidValue > current->getBalance()) {
                std::cout << "[LELANG] Saldo tidak cukup. Maksimal bid: "
                          << current->getBalance() << "\n";
                continue;
            }

            highestBid = bidValue;
            winner = current;
            consecutivePasses = 0;
            break;
        }

        turnIdx = (turnIdx + 1) % activeCount;
    }
}

int AuctionManager::getLastWinningBid() const {
    return lastWinningBid_;
}
