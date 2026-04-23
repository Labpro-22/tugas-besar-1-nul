#include "core/Command.hpp"
#include "core/Dice.hpp"
#include "core/TurnContext.hpp"
#include "board/Board.hpp"
#include "player/Player.hpp"
#include "tile/Tile.hpp"
#include "tile/PropertyTile.hpp"
#include "property/Property.hpp"
#include "property/PropertyStatus.hpp"
#include "exception/InvalidGameStateException.hpp"


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

	const std::string commandName = argv(0);

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
	Tile *baseTile = ctx.board.getTile(nextPos);
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
	out << "[INFO] Command List:\n";
	out << "  - ROLL_DICE\n";
	out << "  - SET_DICE <arg1> <arg2>\n";
	out << "  - HELP\n";
}