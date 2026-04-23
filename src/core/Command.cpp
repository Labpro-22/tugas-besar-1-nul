#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"
#include "player/Player.hpp"
#include "tile/Tile.hpp"
#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"
#include "property/PropertyStatus.hpp"
#include "property/StreetProperty.hpp"
#include "property/RailroadProperty.hpp"
#include "property/UtilityProperty.hpp"
#include "exception/InvalidGameStateException.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace {
std::string toUpperAscii(const std::string& input) {
	std::string result = input;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return static_cast<char>(std::toupper(c)); });
	return result;
}

std::string formatMoneyId(int amount) {
	std::string digits = std::to_string(amount);
	std::string formatted;
	formatted.reserve(digits.size() + (digits.size() / 3));

	int groupCount = 0;
	for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
		formatted.push_back(digits[static_cast<size_t>(i)]);
		++groupCount;
		if (groupCount == 3 && i > 0) {
			formatted.push_back('.');
			groupCount = 0;
		}
	}

	std::reverse(formatted.begin(), formatted.end());
	return formatted;
}

std::string propertyStatusText(PropertyStatus status) {
	if (status == PropertyStatus::MORTGAGED) {
		return "MORTGAGED [M]";
	}
	if (status == PropertyStatus::OWNED) {
		return "OWNED";
	}
	return "BANK";
}
}


void Command::rebuildArgvCache() const {
	argvCache.clear();
	argvCache.reserve(tokens.size());
	for (const std::string& token : tokens) {
		argvCache.push_back(token.c_str());
	}
}

void Command::promptInput(std::istream& in, std::ostream& out) {
	out << "> ";
	std::getline(in, rawInput);
	parse(rawInput);
}

void Command::parse(const std::string& input) {
	rawInput = input;
	tokens.clear();

	std::string current;
	bool inQuotes = false;

	for (size_t i = 0; i < rawInput.size(); ++i) {
		const char ch = rawInput[i];

		if (ch == '"') {
			inQuotes = !inQuotes;
			continue;
		}

		if (!inQuotes && (ch == ' ' || ch == '\t')) {
			if (!current.empty()) {
				tokens.push_back(current);
				current.clear();
			}
			continue;
		}

		current.push_back(ch);
	}

	if (!current.empty()) {
		tokens.push_back(current);
	}

	rebuildArgvCache();
}

int Command::argc() const {
	return static_cast<int>(tokens.size());
}

const char* Command::argv(int index) const {
	if (index < 0 || index >= argc()) {
		return nullptr;
	}
	return argvCache[static_cast<size_t>(index)];
}

const std::vector<std::string>& Command::args() const {
	return tokens;
}

bool Command::dispatch(TurnContext& ctx, std::ostream& out) const {
	if (argc() == 0) {
		out << "[WARN] No Command Written. Type in HELP for command list\n";
		return false;
	}

	const std::string commandName = this->argv(0);

	if (commandName == "ROLL_DICE") {
		execRollDice(ctx, out);
		return true;
	}

	if (commandName == "SET_DICE") {
		execSetDice(ctx, out);
		return true;
	}

	if (commandName == "PRINT_PROP_CERT") {
		execPrintCert(ctx, out);
		return true;
	}

	if (commandName == "PRINT_PROPERTY") {
		execPrintProperty(ctx, out);
		return true;
	}

	if (commandName == "HELP") {
		execHelp(out);
		return true;
	}

	out << "[WARN] Unrecognized command: " << commandName << "\n";
	return false;
}

/* === COMMANDS === */

void Command::execRollDice(TurnContext& ctx, std::ostream& out) const {
	out << "[COMMAND] Rolling dice...\n";

	ctx.dice.roll();
	int diceTotal = ctx.dice.getTotal();

	out << "Result = " << std::to_string(ctx.dice.getDie1()) << "+"
		<< std::to_string(ctx.dice.getDie2()) << " = " << diceTotal << "\n";
	out << "Moving " << ctx.currentPlayer.getUsername() << "'s pawn by " << diceTotal << " steps\n";
	
	int nextPos = ctx.currentPlayer.move(diceTotal, ctx);
	cout << "debug: " << std::to_string(nextPos) << " " << std::to_string(ctx.getBoardSize())<< "\n";
	Tile* baseTile = ctx.board.getTile(nextPos);
	if (baseTile == nullptr) {
		throw InvalidGameStateException("Player moved to an invalid tile index: " + std::to_string(nextPos));
	}

	out << ctx.currentPlayer.getUsername() << " landed in " << baseTile->getName() << "\n";
	baseTile->onLanded(ctx);
}

void Command::execSetDice(TurnContext& ctx, std::ostream& out) const {
	if (argc() < 3) {
		throw InvalidGameStateException("SET_DICE command requires exactly 2 arguments for die values");
	}

	int die1 = std::stoi(argv(1));
	int die2 = std::stoi(argv(2));

	if (die1 < 1 || die1 > 6 || die2 < 1 || die2 > 6) {
		throw InvalidGameStateException("Die values must be between 1 and 6 inclusively");
	}

	out << "[COMMAND] Setting dice...\n";
	ctx.dice.setManual(die1, die2);
	int diceTotal = ctx.dice.getTotal();

	out << "Dice set to: " << die1 << " and " << die2 << "\n"
		<< "Result = " << std::to_string(diceTotal) << "\n";
}

void Command::execPrintCert(TurnContext& ctx, std::ostream& out) const {
	std::string propertyCode;

	if (argc() >= 2 && argv(1) != nullptr) {
		propertyCode = argv(1);
	} else {
		out << "[COMMAND] Input property code: ";
		std::getline(std::cin, propertyCode);
	}

	if (propertyCode.empty()) {
		out << "[WARN] Property code cannot be empty.\n";
		return;
	}

	// ini pake pointer tiati dah
	Tile* tile = ctx.board.getTileByCode(propertyCode);
	if (tile == nullptr) {
		out << "[WARN] Property with code '" << propertyCode << "' not found.\n";
		return;
	}

	PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(tile);
	if (propertyTile == nullptr) {
		out << "[WARN] Tile with code '" << propertyCode << "' is not a property tile.\n";
		return;
	}

	Property* property = propertyTile->getProperty();
	if (property == nullptr) {
		out << "[WARN] Property data for code '" << propertyCode << "' is unavailable.\n";
		return;
	}

	out << "[INFO] Property Certificate\n";
	out << "Code  : " << property->getCode() << "\n";
	out << "Name  : " << property->getName() << "\n";

	const PropertyStatus status = property->getStatus();
	if (status == PropertyStatus::BANK) {
		out << "Status: BANK\n";
		out << "Owner : BANK\n";
		return;
	}

	if (status == PropertyStatus::MORTGAGED) {
		out << "Status: MORTGAGED\n";
	} else {
		out << "Status: OWNED\n";
	}

	Player* owner = property->getOwner();
	if (owner == nullptr) {
		out << "Owner : UNKNOWN\n";
		return;
	}

	out << "Owner : " << owner->getUsername() << "\n";
}

void Command::execPrintProperty(TurnContext& ctx, std::ostream& out) const {
	Player &curPlayer = ctx.currentPlayer;
	const auto& properties = curPlayer.getProperties();

	out << "[COMMAND] Checking properties of...\n";
	out << "==== " << curPlayer.getUsername() << " ====\n";
	if (properties.empty()) {
		out << "You don't have any properties.\n";
		return;
	}

	struct PropertyLine {
		std::string name;
		std::string code;
		std::string buildingInfo;
		std::string statusText;
		int baseValue;
	};

	std::unordered_map<std::string, std::vector<PropertyLine>> grouped;
	std::vector<std::string> groupOrder;
	int totalWealth = 0;

	for (Property* prop : properties) {
		if (prop == nullptr) {
			continue;
		}

		std::string groupName = "LAINNYA";
		std::string buildingInfo;

		if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
			groupName = toUpperAscii(street->getColorGroup());
			if (street->hasHotel()) {
				buildingInfo = "Hotel";
			} else if (street->getBuildingCount() > 0) {
				buildingInfo = std::to_string(street->getBuildingCount()) + " rumah";
			}
		} else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
			groupName = "STATION";
		} else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
			groupName = "UTIL";
		}

		if (grouped.find(groupName) == grouped.end()) {
			groupOrder.push_back(groupName);
		}

		grouped[groupName].push_back(PropertyLine{
			prop->getName(),
			prop->getCode(),
			buildingInfo,
			propertyStatusText(prop->getStatus()),
			prop->getBuyPrice()
		});

		totalWealth += prop->getBuyPrice();
	}

	out << "=== Properties of: " << curPlayer.getUsername() << " ===\n";
	for (const std::string& groupName : groupOrder) {
		out << "\n[" << groupName << "]\n";
		const auto& lines = grouped[groupName];
		for (const PropertyLine& line : lines) {
			out << "  - " << line.name << " (" << line.code << ")";
			if (!line.buildingInfo.empty()) {
				out << "  " << line.buildingInfo;
			}
			out << "  M" << line.baseValue << "  " << line.statusText << "\n";
		}
	}

	out << "\nTotal kekayaan properti: M" << formatMoneyId(totalWealth) << "\n";

}

void Command::execMortgage(TurnContext& ctx, std::ostream& out) const {

}

void Command::execDismortgage(TurnContext& ctx, std::ostream& out) const {

}

void Command::execUpgrade(TurnContext& ctx, std::ostream& out) const {

}

void Command::execSave(TurnContext& ctx, std::ostream& out) const {

}

void Command::execLoad(TurnContext& ctx, std::ostream& out) const {

}

void Command::execPrintLog(TurnContext& ctx, std::ostream& out) const {

}

void Command::execUseSkill(TurnContext& ctx, std::ostream& out) const {

}

void Command::execHelp(std::ostream& out) const {
	out << "\n";
    out << "╔══════════════════════════════════════════════════════════════════╗\n";
    out << "║                        P A N D U A N                             ║\n";
    out << "╠══════════════════════════════════════════════════════════════════╣\n";
    out << "║ Nimonspoli adalah permainan papan strategi yang terinspirasi     ║\n";
    out << "║ dari Monopoly. Pemain bergerak mengelilingi papan, membeli       ║\n";
    out << "║ properti, membangun rumah/hotel, dan memungut sewa.              ║\n";
    out << "║                                                                  ║\n";
    out << "║ Tujuan: Menjadi pemain terkaya atau satu-satunya yang tersisa    ║\n";
    out << "║ setelah pemain lain bangkrut.                                    ║\n";
    out << "║                                                                  ║\n";
    out << "║ MODE BOT (Bonus):                                                ║\n";
    out << "║   - Anda bisa bermain melawan 1-3 bot/komputer                   ║\n";
    out << "║   - Bot level Medium akan menjadi lawan Anda                     ║\n";
    out << "║   - Bot akan mengambil keputusan secara otomatis                 ║\n";
    out << "║                                                                  ║\n";
    out << "║ Dalam permainan, Anda dapat menggunakan perintah:                ║\n";
    out << "║   - ROLL_DICE      : Lempar dadu untuk bergerak                  ║\n";
    out << "║   - PRINT_BOARD    : Lihat papan permainan                       ║\n";
    out << "║   - PRINT_PROP     : Lihat properti Anda                         ║\n";
    out << "║   - MORTGAGE       : Gadai properti ke bank                      ║\n";
    out << "║   - BUILD          : Bangun rumah/hotel                          ║\n";
    out << "║   - dan banyak lagi...                                           ║\n";
    out << "╚══════════════════════════════════════════════════════════════════╝\n";
}