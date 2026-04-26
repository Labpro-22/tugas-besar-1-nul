#include "core/Command.hpp"
#include "board/Board.hpp"
#include "core/Dice.hpp"
#include "core/GameEngine.hpp"
#include "core/TurnContext.hpp"
#include "core/TurnManager.hpp"
#include "exception/InvalidGameStateException.hpp"
#include "player/Player.hpp"
#include "property/Property.hpp"
#include "property/PropertyStatus.hpp"
#include "property/RailroadProperty.hpp"
#include "property/StreetProperty.hpp"
#include "property/UtilityProperty.hpp"
#include "tile/PropertyTile.hpp"
#include "tile/ActionTile.hpp"
#include "tile/Tile.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <unordered_map>

namespace {
std::string toUpperAscii(const std::string& input) {
    std::string result = input;
    std::transform(
        result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
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

std::string padRight(const std::string& text, size_t width) {
    if (text.size() >= width) {
        return text;
    }
    return text + std::string(width - text.size(), ' ');
}

std::pair<std::string, std::string> tileColorCodeAndAnsi(Tile* tile) {
    if (tile == nullptr) {
        return {"DF", "\033[37m"};
    }

    if (dynamic_cast<UtilityTile*>(tile) != nullptr) {
        return {"AB", "\033[96m"};
    }

	if (PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile)) {
		if (StreetProperty* street = dynamic_cast<StreetProperty*>(propTile->getProperty())) {
			const std::string group = toUpperAscii(street->getColorGroup());
			if (group == "COKLAT") return {"CK", "\033[33m"};
			if (group == "BIRU MUDA") return {"BM", "\033[94m"};
			if (group == "MERAH MUDA") return {"PK", "\033[95m"};
			if (group == "ORANGE") return {"OR", "\033[91m"};
			if (group == "MERAH") return {"MR", "\033[31m"};
			if (group == "KUNING") return {"KN", "\033[93m"};
			if (group == "HIJAU") return {"HJ", "\033[32m"};
			if (group == "BIRU TUA") return {"BT", "\033[34m"};
		}
	}

    return {"DF", "\033[37m"};
}

bool shouldAdvanceTurnAfterJail(const TurnContext& ctx) {
    return ctx.currentPlayer.isInJail() && !ctx.startedTurnInJail;
}

bool handleGoToJailLanding(Tile* landedTile, TurnContext& ctx, std::ostream& out) {
    if (dynamic_cast<GoToJailTile*>(landedTile) == nullptr) {
        return false;
    }

    out << "[INFO] Mendarat di Go To Jail. Pemain dipindahkan ke penjara.\n";
    ctx.currentPlayer.moveForwardTo(10, ctx);

    Tile* jailBaseTile = ctx.board.getTile(10);
    JailTile* jailTile = dynamic_cast<JailTile*>(jailBaseTile);
    if (jailTile == nullptr) {
        throw InvalidGameStateException("Error when moving player to Jail from GoToJail tile!");
    }

    jailBaseTile->onLanded(ctx);
    if (!ctx.currentPlayer.isInJail()) {
        ctx.currentPlayer.enterJail();
    }
    if (!jailTile->isInmate(ctx.currentPlayer)) {
        jailTile->addInmate(ctx.currentPlayer);
    }

    ctx.gameEngine.logAction(ctx.currentPlayer.getUsername(), "PENJARA", "Mendarat di Go To Jail");
    ctx.gameEngine.getTurnManager().resetDoubleGotten();
    return true;
}
} // namespace

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
	if (ctx.dice.getDie1() == 0 && ctx.dice.getDie2() == 0) {
		this->canEndTurn = false;
	}

	if (argc() == 0) {
		out << "[WARN] No Command Written. Type in HELP for command list\n\n";
		return false;
	}

    const std::string commandName = toUpperAscii(this->argv(0));

	if (commandName == "END_TURN") {
		if (!ctx.canEndTurn()) {
			if (ctx.currentPlayer.isInJail()) {
				out << "[WARN] You are currently in jail! To escape, either: Pay the Fine or use ROLL_DICE to roll a double.\n";
			} else {
				out << "[WARN] You haven't rolled the dice! Use ROLL_DICE before ending your turn.\n";
			}
            out << "\n";
			return false;
		}
        out << "\n";
		return true;
	}
    else if (commandName == "ROLL_DICE") {
        execRollDice(ctx, out);
        if (shouldAdvanceTurnAfterJail(ctx)) {
            return true;
        }
    }
    else if (commandName == "SET_DICE") {
        execSetDice(ctx, out);
        if (shouldAdvanceTurnAfterJail(ctx)) {
            return true;
        }
    }
    else if (commandName == "PAY_JAIL_FEE") execPayJailFee(ctx, out);
	else if (commandName == "PRINT_BOARD") execPrintBoard(ctx, out);
	else if (commandName == "PRINT_PROP_CERT") execPrintCert(ctx, out);
	else if (commandName == "PRINT_PROPERTY") execPrintProperty(ctx, out);
	else if (commandName == "MORTGAGE") execMortgage(ctx, out);
	else if (commandName == "PROFILE") execProfile(ctx, out);
	else if (commandName == "USE_SKILL") execUseSkill(ctx, out);
	else if (commandName == "BUILD") execUpgrade(ctx, out);
	else if (commandName == "PRINT_LOG") execPrintLog(ctx, out);
	else if (commandName == "SAVE") execSave(ctx, out);
	else if (commandName == "HELP") execHelp(out);
	else {
        out << "[WARN] Unrecognized command: " << commandName << "\n";
        out << "Type HELP to see the list of available commands.\n\n";}
	return false;
}

/* === COMMANDS === */

void Command::execRollDice(TurnContext& ctx, std::ostream& out) const {
    if (ctx.currentPlayer.isInJail()
        && ctx.gameEngine.getTurnManager().getHasActedThisTurn()){
        out << "should not ever show up: [WARN] You have played in this turn, and you are still in jail.\n";
        return;
    }

    out << "[COMMAND] Rolling Dice\n\n";

	bool validRoll = ctx.dice.roll();
	if (!validRoll) {
		out << "[WARN] You cannot roll the dice anymore this turn.\n";
		return;
	}

	
    bool movePlayerToJail = false;
    bool movePlayerOutOfJail = false;

	// Mark that player has taken action (setting dice)
	ctx.gameEngine.getTurnManager().markActionTaken();

	if (ctx.dice.isDouble()) {
        ctx.gameEngine.getTurnManager().incrementDoubleGotten();
        int currentDoubles = ctx.gameEngine.getTurnManager().getDoubleGotten();

        
        if (ctx.currentPlayer.getStatus() == PlayerStatus::JAILED){
            out << "You got double dice, you got out of jail!\n";
            movePlayerOutOfJail = true;
        }

        if (currentDoubles >= 3) {
            out << "Go to jail because going over speed limit\n"; 
            
            // Log tindakan ke TransactionLogger
            ctx.gameEngine.logAction(ctx.currentPlayer.getUsername(), "PENJARA", "3x Double (Speeding)");
            movePlayerToJail = true; // flag yang memindahkan player ke jail
            // Reset counter karena giliran berakhir secara paksa
            ctx.gameEngine.getTurnManager().resetDoubleGotten();
        }
        out << "[INFO] Double! Anda mendapatkan kesempatan melempar lagi.\n";
    } else {
        // Perbaikan: Reset counter jika lemparan saat ini bukan double
        ctx.gameEngine.getTurnManager().resetDoubleGotten();
    }

	// Set flag untuk validasi END_TURN
	if (ctx.currentPlayer.isInJail()) {
		ctx.hasTakenJailAction = true;
	} else {
		ctx.hasRolled = true;
	}

	int diceTotal = ctx.dice.getTotal();
	this->canEndTurn = !ctx.dice.isDouble();


	out << "Result = " << std::to_string(ctx.dice.getDie1()) << "+"
		<< std::to_string(ctx.dice.getDie2()) << " = " << diceTotal << "\n";

    if (movePlayerOutOfJail){
	    Tile* baseTile = ctx.board.getTile(10);
        JailTile* jt = dynamic_cast<JailTile*> (baseTile);
        if (jt == nullptr){
            throw InvalidGameStateException("Error when moving player out of Jail!");
            return;
        } else{
            ctx.currentPlayer.exitJail();
            jt->removeInmate(ctx.currentPlayer);
        }
    }

    if (movePlayerToJail){
	    ctx.currentPlayer.moveForwardTo(10, ctx); // 10 index jail
	    Tile* baseTile = ctx.board.getTile(10);
        baseTile->onLanded(ctx);
        JailTile* jt = dynamic_cast<JailTile*> (baseTile);
        if (jt == nullptr){
            throw InvalidGameStateException("Error when moving player to Jail!");
            return;
        } else{
            ctx.currentPlayer.enterJail();
            jt->addInmate(ctx.currentPlayer);
            return;
        }
    }

    if (ctx.currentPlayer.getStatus() == PlayerStatus::JAILED){
        if (ctx.currentPlayer.getJailTurns() == 0){
            int jail_fee = ctx.gameEngine.getJailFine();
            ctx.currentPlayer.deductCash(jail_fee);
            out << "Player " << ctx.currentPlayer.getUsername() << " paid M" << jail_fee << " to get out of jail!\n";
            Tile* baseTile = ctx.board.getTile(10);
            JailTile* jt = dynamic_cast<JailTile*> (baseTile);
            if (jt == nullptr){
                throw InvalidGameStateException("Error when moving player out of Jail!");
                return;
            } else{
                ctx.currentPlayer.exitJail();
                jt->removeInmate(ctx.currentPlayer);
                return;
            }
        }
        ctx.currentPlayer.decrementjailTurns();
        out << "You failed to roll doubles. Turns in jail remaining: " << ctx.currentPlayer.getJailTurns() << "\n";
        return;
    }

	out << "Moving " << ctx.currentPlayer.getUsername() << "'s pawn by " << diceTotal << " steps\n";

	// Log dice roll
	std::string logDetail = "Lempar: " + std::to_string(ctx.dice.getDie1()) + "+" +
	                        std::to_string(ctx.dice.getDie2()) + "=" + std::to_string(diceTotal);
	ctx.gameEngine.logAction(ctx.currentPlayer.getUsername(), "DADU", logDetail);

	int nextPos = ctx.currentPlayer.move(diceTotal, ctx);
	Tile* baseTile = ctx.board.getTile(nextPos);
	if (baseTile == nullptr) {
		throw InvalidGameStateException("Player moved to an invalid tile index: " + std::to_string(nextPos));
	}

	// out << ctx.currentPlayer.getUsername() << " landed in " << baseTile->getName() << "\n";

	// Log movement
	logDetail = "Mendarat di " + baseTile->getName() + " (" + baseTile->getCode() + ")";
	ctx.gameEngine.logAction(ctx.currentPlayer.getUsername(), "GERAK", logDetail);

    if (handleGoToJailLanding(baseTile, ctx, out)) {
        return;
    }

	baseTile->onLanded(ctx);

	if (ctx.currentPlayer.isShieldActive()) ctx.currentPlayer.deactivateShield();
}

void Command::execSetDice(TurnContext& ctx, std::ostream& out) const {
    if (ctx.currentPlayer.isInJail()
        && ctx.gameEngine.getTurnManager().getHasActedThisTurn()){
        out << "[WARN] You have played in this turn, and you are still in jail.\n";
        return;
    }

    if (!ctx.dice.canRoll){
		out << "[WARN] You cannot roll the dice anymore this turn.\n";
		return;
	}
    if (argc() < 3) {
        throw InvalidGameStateException(
            "SET_DICE command requires exactly 2 arguments for die values");
    }

    int die1 = std::stoi(argv(1));
    int die2 = std::stoi(argv(2));

    if (die1 < 1 || die1 > 6 || die2 < 1 || die2 > 6) {
        throw InvalidGameStateException(
            "Die values must be between 1 and 6 inclusively");
    }

	out << "[COMMAND] Setting dice...\n";

    bool movePlayerToJail = false;
    bool movePlayerOutOfJail = false;

	// Mark that player has taken action (setting dice)
	ctx.gameEngine.getTurnManager().markActionTaken();
    ctx.dice.setManual(die1, die2);

	if (ctx.dice.isDouble()) {
        ctx.gameEngine.getTurnManager().incrementDoubleGotten();
        int currentDoubles = ctx.gameEngine.getTurnManager().getDoubleGotten();

        
        if (ctx.currentPlayer.getStatus() == PlayerStatus::JAILED){
            out << "You got double dice!\n";
            movePlayerOutOfJail = true;
        }

        if (currentDoubles >= 3) {
            out << "Go to jail because going over speed limit\n"; 
            
            // Log tindakan ke TransactionLogger
            ctx.gameEngine.logAction(ctx.currentPlayer.getUsername(), "PENJARA", "3x Double (Speeding)");
            movePlayerToJail = true; // flag yang memindahkan player ke jail
            // Reset counter karena giliran berakhir secara paksa
            ctx.gameEngine.getTurnManager().resetDoubleGotten();
        } else{
            out << "[INFO] Double! You get a chance to throw again\n";
        }
        
    } else {
        // Perbaikan: Reset counter jika lemparan saat ini bukan double
        ctx.gameEngine.getTurnManager().resetDoubleGotten();
    }

	// Set flag untuk validasi END_TURN
	if (ctx.currentPlayer.isInJail()) {
		ctx.hasTakenJailAction = true;
	} else {
		ctx.hasRolled = true;
	}

	int diceTotal = ctx.dice.getTotal();
	this->canEndTurn = !ctx.dice.isDouble();

	out << "Dice set to: " << die1 << " and " << die2 << "\n";

	out << "Result = " << std::to_string(ctx.dice.getDie1()) << "+"
		<< std::to_string(ctx.dice.getDie2()) << " = " << diceTotal << "\n";

    if (movePlayerOutOfJail){
	    Tile* baseTile = ctx.board.getTile(10);
        JailTile* jt = dynamic_cast<JailTile*> (baseTile);
        if (jt == nullptr){
            throw InvalidGameStateException("Error when moving player out of Jail!");
            return;
        } else{
            ctx.currentPlayer.exitJail();
            jt->removeInmate(ctx.currentPlayer);
        }
    }

    if (movePlayerToJail){
	    ctx.currentPlayer.moveForwardTo(10, ctx); // 10 index jail
	    Tile* baseTile = ctx.board.getTile(10);
        baseTile->onLanded(ctx);
        JailTile* jt = dynamic_cast<JailTile*> (baseTile);
        if (jt == nullptr){
            throw InvalidGameStateException("Error when moving player to Jail!");
            return;
        } else{
            ctx.currentPlayer.enterJail();
            jt->addInmate(ctx.currentPlayer);
            return;
        }
    }

    if (ctx.currentPlayer.getStatus() == PlayerStatus::JAILED){
        if (ctx.currentPlayer.getJailTurns() == 0){
            int jail_fee = ctx.gameEngine.getJailFine();
            ctx.currentPlayer.deductCash(jail_fee);
            out << "Player " << ctx.currentPlayer.getUsername() << " paid M" << jail_fee << " to get out of jail!\n";
            Tile* baseTile = ctx.board.getTile(10);
            JailTile* jt = dynamic_cast<JailTile*> (baseTile);
            if (jt == nullptr){
                throw InvalidGameStateException("Error when moving player out of Jail!");
                return;
            } else{
                ctx.currentPlayer.exitJail();
                jt->removeInmate(ctx.currentPlayer);
                return;
            }
        }
        ctx.currentPlayer.decrementjailTurns();
        out << "You failed to roll doubles. Turns in jail remaining: " << ctx.currentPlayer.getJailTurns() << "\n";
        return;
    }
	out << "Moving " << ctx.currentPlayer.getUsername() << "'s pawn by " << diceTotal << " steps\n\n";
	
	int nextPos = ctx.currentPlayer.move(diceTotal, ctx);
	Tile* baseTile = ctx.board.getTile(nextPos);
	if (baseTile == nullptr) {
		throw InvalidGameStateException("Player moved to an invalid tile index: " + std::to_string(nextPos));
	}

	// out << ctx.currentPlayer.getUsername() << " landed in " << baseTile->getName() << "\n";
    if (handleGoToJailLanding(baseTile, ctx, out)) {
        return;
    }
	baseTile->onLanded(ctx);

	if (ctx.currentPlayer.isShieldActive()) ctx.currentPlayer.deactivateShield();
}

void Command::execPrintCert(TurnContext& ctx, std::ostream& out) const {
    // GUI mode: show property cert panel
    if (auto* pm = ctx.gameEngine.getPanelManager()) {
        pm->showPropertyCert(ctx.board);
        return;
    }

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
        out << "[WARN] Property with code '" << propertyCode
            << "' not found.\n";
        return;
    }

    PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(tile);
    if (propertyTile == nullptr) {
        out << "[WARN] Tile with code '" << propertyCode
            << "' is not a property tile.\n";
        return;
    }

	Property* property = propertyTile->getProperty();
	if (property == nullptr) {
		out << "[WARN] Property data for code '" << propertyCode << "' is unavailable.\n";
		return;
	}
	
	property->printStatus(ctx);

	// out << "[INFO] Property Certificate\n";
	// out << "Code  : " << property->getCode() << "\n";
	// out << "Name  : " << property->getName() << "\n";

	// const PropertyStatus status = property->getStatus();
	// if (status == PropertyStatus::BANK) {
	// 	out << "Status: BANK\n";
	// 	out << "Owner : BANK\n";
	// 	return;
	// }

	// if (status == PropertyStatus::MORTGAGED) {
	// 	out << "Status: MORTGAGED\n";
	// } else {
	// 	out << "Status: OWNED\n";
	// }

	// Player* owner = property->getOwner();
	// if (owner == nullptr) {
	// 	out << "Owner : UNKNOWN\n";
	// 	return;
	// }

	// out << "Owner : " << owner->getUsername() << "\n";
}

void Command::execPrintProperty(TurnContext& ctx, std::ostream& out) const {
    // GUI mode: show properties list panel
    if (auto* pm = ctx.gameEngine.getPanelManager()) {
        pm->showPropertiesList(ctx.currentPlayer);
        return;
    }

    Player& curPlayer = ctx.currentPlayer;
    const auto& properties = curPlayer.getProperties();

    out << "[COMMAND] Checking properties...\n";
    if (properties.empty()) {
        out << "You don't have any properties.\n";
        return;
    }

	out << "=== Properties of: " << curPlayer.getUsername() << " ===\n";
	out << "\nStreet Properties\n";
	for (Property* prop : properties){
		if (prop->getOwner() == &ctx.currentPlayer && dynamic_cast<StreetProperty*> (prop) != nullptr){
			prop->printStatus(ctx);
			cout << "\n";
		}
	}

	out << "\nRailroad Properties\n";
	for (Property* prop : properties){
		if (prop->getOwner() == &ctx.currentPlayer && dynamic_cast<RailroadProperty*> (prop) != nullptr){
			prop->printStatus(ctx);
			cout << "\n";
		}
	}

	out << "\nUtility Properties\n";
	for (Property* prop : properties){
		if (prop->getOwner() == &ctx.currentPlayer && dynamic_cast<UtilityProperty*> (prop) != nullptr){
			prop->printStatus(ctx);
			cout << "\n";
		}
	}
}

void Command::execPayJailFee(TurnContext& ctx, std::ostream& out) const{
    if (ctx.currentPlayer.getStatus() != PlayerStatus::JAILED){
        out << "You are not in jail! Cannot pay jail fee!\n";
        return;
    }
    int jail_fee = ctx.gameEngine.getJailFine(); 
    ctx.currentPlayer.deductCash(jail_fee);
    out << "Player " << ctx.currentPlayer.getUsername() << " paid M" << jail_fee << " to get out of jail!\n";
    Tile* baseTile = ctx.board.getTile(10);
    JailTile* jt = dynamic_cast<JailTile*> (baseTile);
    if (jt == nullptr){
        throw InvalidGameStateException("Error when moving player out of Jail!");
        return;
    } else{
        ctx.currentPlayer.exitJail();
        jt->removeInmate(ctx.currentPlayer);
        return;
    }
}

	// struct PropertyLine {
	// 	std::string name;
	// 	std::string code;
	// 	std::string buildingInfo;
	// 	std::string statusText;
	// 	int baseValue;
	// };

	// std::unordered_map<std::string, std::vector<PropertyLine>> grouped;
	// std::vector<std::string> groupOrder;
	// int totalWealth = 0;

	// for (Property* prop : properties) {
	// 	if (prop == nullptr) {
	// 		continue;
	// 	}

	// 	std::string groupName = "LAINNYA";
	// 	std::string buildingInfo;

	// 	if (StreetProperty* street = dynamic_cast<StreetProperty*>(prop)) {
	// 		groupName = toUpperAscii(street->getColorGroup());
	// 		if (street->hasHotel()) {
	// 			buildingInfo = "Hotel";
	// 		} else if (street->getBuildingCount() > 0) {
	// 			buildingInfo = std::to_string(street->getBuildingCount()) + " rumah";
	// 		}
	// 	} else if (dynamic_cast<RailroadProperty*>(prop) != nullptr) {
	// 		groupName = "STATION";
	// 	} else if (dynamic_cast<UtilityProperty*>(prop) != nullptr) {
	// 		groupName = "UTIL";
	// 	}

	// 	if (grouped.find(groupName) == grouped.end()) {
	// 		groupOrder.push_back(groupName);
	// 	}

	// 	grouped[groupName].push_back(PropertyLine{
	// 		prop->getName(),
	// 		prop->getCode(),
	// 		buildingInfo,
	// 		propertyStatusText(prop->getStatus()),
	// 		prop->getBuyPrice()
	// 	});

	// 	totalWealth += prop->getBuyPrice();
	// }

    // out << "\nTotal kekayaan properti: M" << formatMoneyId(totalWealth) << "\n";

void Command::execProfile(TurnContext& ctx, std::ostream& out) const {
	// GUI mode: show profile panel
	if (auto* pm = ctx.gameEngine.getPanelManager()) {
		pm->showProfile(ctx.currentPlayer);
		return;
	}

	Player& currentPlayer = ctx.currentPlayer;
	const auto& properties = currentPlayer.getProperties();

	int totalHousesOwned = 0;
	int totalHotelsOwned = 0;

	for (Property* property : properties) {
		if (property == nullptr) {
			continue;
		}

		StreetProperty* streetProperty = dynamic_cast<StreetProperty*>(property);
		if (streetProperty == nullptr) {
			continue;
		}

		if (streetProperty->hasHotel()) {
			++totalHotelsOwned;
		} else {
			totalHousesOwned += streetProperty->getBuildingCount();
		}
	}

    out << "\n";
	out << "=== PLAYER PROFILE ===\n";
	out << "Name                   : " << currentPlayer.getUsername() << "\n";
	out << "Balance                : M" << formatMoneyId(currentPlayer.getBalance()) << "\n";
	out << "Total properties owned : " << currentPlayer.getPropertiesAmount() << "\n";
	out << "Total houses owned     : " << totalHousesOwned << "\n";
	out << "Total hotels owned     : " << totalHotelsOwned << "\n\n";
}

void Command::execMortgage(TurnContext& ctx, std::ostream& out) const {
    out << "\n";
    out << "[COMMAND] Mortgage\n\n";

	Player& currentPlayer = ctx.currentPlayer;
	if (currentPlayer.getPropertiesAmount() == 0) {
		out << "[WARN] You do not own any properties to mortgage.\n\n";
		return;
	}

	// GUI mode: show mortgage panel
	if (auto* pm = ctx.gameEngine.getPanelManager()) {
		pm->showMortgage(currentPlayer);
		return;
	}

	std::string propertyCode;
	if (argc() >= 2 && argv(1) != nullptr) {
		propertyCode = argv(1);
	} else {
		out << "Input property code to mortgage\n";
        out << "> ";
		std::getline(std::cin, propertyCode);
	}

	if (propertyCode.empty()) {
		out << "[WARN] Property code cannot be empty.\n\n";
		return;
	}

	Tile* tile = ctx.board.getTileByCode(propertyCode);
	if (tile == nullptr) {
		out << "[WARN] Property with code '" << propertyCode << "' not found.\n\n";
		return;
	}

	PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(tile);
	if (propertyTile == nullptr) {
		out << "[WARN] Tile with code '" << propertyCode << "' is not a property tile.\n\n";
		return;
	}

	Property* tileProperty = propertyTile->getProperty();
	if (tileProperty == nullptr) {
		throw InvalidGameStateException("Property data is unavailable for tile code: " + propertyCode);
	}

	const auto& ownedProperties = currentPlayer.getProperties();
	auto ownedIt = std::find(ownedProperties.begin(), ownedProperties.end(), tileProperty);
	if (ownedIt == ownedProperties.end()) {
		out << "[WARN] You do not own property '" << tileProperty->getName() << "'.\n\n";
		return;
	}

	if (tileProperty->getOwner() != &currentPlayer) {
		throw InvalidGameStateException(
			"Ownership mismatch detected: player property list and tile owner are inconsistent for " +
			tileProperty->getCode());
	}

	if (tileProperty->getStatus() == PropertyStatus::MORTGAGED) {
		out << "[WARN] Property '" << tileProperty->getName() << "' is already mortgaged.\n\n";
		return;
	}

	currentPlayer.mortgage(tileProperty);

	if (tileProperty->getStatus() != PropertyStatus::MORTGAGED) {
		throw InvalidGameStateException(
			"Mortgage status update failed for property code: " + tileProperty->getCode());
	}

	out << "[INFO] Property '" << tileProperty->getName() << "' has been mortgaged.\n";
	out << "[INFO] You received M" << formatMoneyId(tileProperty->getMortgageValue()) << ".\n\n";
}

void Command::execDismortgage(TurnContext& ctx, std::ostream& out) const {
    out << "[COMMAND] Dismortgage\n\n";

    Player& currentPlayer = ctx.currentPlayer;

    // GUI mode: show dismortgage panel
    if (auto* pm = ctx.gameEngine.getPanelManager()) {
        pm->showDismortgage(currentPlayer);
        return;
    }

    std::string propertyCode;
    if (argc() >= 2 && argv(1) != nullptr) {
        propertyCode = argv(1);
    } else {
        out << "[COMMAND] Input property code to dismortgage: ";
        std::getline(std::cin, propertyCode);
    }

    if (propertyCode.empty()) {
        out << "[WARN] Property code cannot be empty.\n\n";
        return;
    }

    Tile* tile = ctx.board.getTileByCode(propertyCode);
    if (tile == nullptr) {
        out << "[WARN] Property with code '" << propertyCode << "' not found.\n\n";
        return;
    }

    PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(tile);
    if (propertyTile == nullptr) {
        out << "[WARN] Tile with code '" << propertyCode << "' is not a property tile.\n\n";
        return;
    }

    Property* tileProperty = propertyTile->getProperty();
    if (tileProperty == nullptr) {
        throw InvalidGameStateException("Property data is unavailable for tile code: " + propertyCode);
    }

    const auto& ownedProperties = currentPlayer.getProperties();
    auto ownedIt = std::find(ownedProperties.begin(), ownedProperties.end(), tileProperty);
    if (ownedIt == ownedProperties.end()) {
        out << "[WARN] You do not own property '" << tileProperty->getName() << "'.\n\n";
        return;
    }

    if (tileProperty->getOwner() != &currentPlayer) {
        throw InvalidGameStateException(
            "Ownership mismatch detected: player property list and tile owner are inconsistent for " +
            tileProperty->getCode());
    }

    if (tileProperty->getStatus() != PropertyStatus::MORTGAGED) {
        out << "[WARN] Property '" << tileProperty->getName() << "' is not mortgaged.\n\n";
        return;
    }

    int cost = static_cast<int>(std::ceil(tileProperty->getMortgageValue() * 1.1f));
    currentPlayer.deductCash(cost);
    tileProperty->redeem();

    out << "[INFO] Property '" << tileProperty->getName() << "' has been dismortgaged.\n";
    out << "[INFO] You paid M" << formatMoneyId(cost) << ".\n\n";
}

void Command::execUpgrade(TurnContext& ctx, std::ostream& out) const {
	ctx.currentPlayer.upgrade(ctx);
}

void Command::execSave(TurnContext& ctx, std::ostream& out) const {
    out << "[COMMAND] Save Game\n\n";

    if (ctx.gameEngine.getTurnManager().getHasActedThisTurn()) {
        out << "[WARN] Save can only be done at the start of a turn, before writing any commands.\n\n";
        return;
    }

    // GUI mode: show save panel
    if (auto* pm = ctx.gameEngine.getPanelManager()) {
        pm->showSave(ctx.gameEngine);
        return;
    }

    std::string filename;

    if (argc() >= 2 && argv(1) != nullptr) {
        filename = argv(1);
    } else {
        out << "Input filename:\n";
        out << "> ";
        std::getline(std::cin, filename);
    }

    if (filename.empty()) {
        out << "[WARN] Filename cannot be empty.\n\n";
        return;
    }

    try {
        ctx.gameEngine.saveGame(filename);
    } catch (const std::exception& e) {
        out << "[ERROR] Failed to save game: " << e.what() << "\n";
    }
}

// void Command::execLoad(TurnContext& ctx, std::ostream& out) const {

// }

void Command::execPrintLog(TurnContext& ctx, std::ostream& out) const {
    // GUI mode: show log panel
    if (auto* pm = ctx.gameEngine.getPanelManager()) {
        pm->showLog(ctx.gameEngine.getLogger());
        return;
    }
    ctx.gameEngine.printLogs();
}

void Command::execUseSkill(TurnContext& ctx, std::ostream& out) const {
	out << "[COMMAND] Skill Card Menu\n\n";
	if (ctx.currentPlayer.getHandsAmount() == 0) {
		out << "No cards found in hand\n";
		return;
	}

	// GUI mode: show skill card panel
	if (auto* pm = ctx.gameEngine.getPanelManager()) {
		pm->showSkillCard(ctx.currentPlayer, ctx);
		return;
	}

	ctx.currentPlayer.showHands();

	std::string selectedIndexText;
	if (argc() >= 2 && argv(1) != nullptr) {
		selectedIndexText = argv(1);
	} else {
		out << "Enter your desired skill card's index\n";
		out << "> ";
		std::getline(std::cin, selectedIndexText);
	}

	int selectedIndex = 0;
	try {
		selectedIndex = std::stoi(selectedIndexText);
	} catch (const std::exception&) {
		throw InvalidGameStateException("Skill card index must be a valid integer");
	}

	ctx.currentPlayer.useSCard(selectedIndex, ctx);
	out << "[INFO] Skill card index " << selectedIndex << " used successfully\n";
}

void Command::execHelp(std::ostream& out) const {
	out << "[Command] Help Page\n\n";
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
    out << "║ PERINTAH GERAK:                                                  ║\n";
    out << "║   - ROLL_DICE      : Lempar dadu untuk bergerak                  ║\n";
    out << "║   - SET_DICE X Y   : Atur dadu manual (contoh: SET_DICE 3 5)     ║\n";
    out << "║   - END_TURN       : Akhiri giliran (wajib lempar dadu dulu!)    ║\n";
    out << "║                                                                  ║\n";
    out << "║ PERINTAH INFO:                                                   ║\n";
    out << "║   - PRINT_BOARD    : Lihat papan permainan                       ║\n";
    out << "║   - PRINT_PROPERTY : Lihat properti Anda                         ║\n";
    out << "║   - PRINT_PROP_CERT [KODE] : Lihat akta properti                 ║\n";
    out << "║   - PROFILE        : Lihat profil pemain                         ║\n";
    out << "║   - PRINT_LOG      : Lihat log transaksi                         ║\n";
    out << "║                                                                  ║\n";
    out << "║ PERINTAH AKSI:                                                   ║\n";
    out << "║   - MORTGAGE       : Gadai properti ke bank                      ║\n";
    out << "║   - BUILD          : Bangun rumah/hotel di properti              ║\n";
    out << "║   - USE_SKILL      : Gunakan kartu kemampuan                     ║\n";
    out << "║                                                                  ║\n";
    out << "║ PERINTAH LAIN:                                                   ║\n";
    out << "║   - SAVE           : Simpan permainan                            ║\n";
    out << "║   - HELP           : Tampilkan panduan ini                       ║\n";
    out << "╚══════════════════════════════════════════════════════════════════╝\n\n";
}

void Command::execPrintBoard(TurnContext& ctx, std::ostream& out) const {
    out << "[COMMAND] Print Board\n\n";

    struct CellContent {
        std::string line1Plain;
        std::string line1Rendered;
        std::string line2;
    };

    const std::vector<Player*> players = ctx.gameEngine.getPlayers();
    std::unordered_map<const Player*, int> playerIndex;
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i] != nullptr) {
            playerIndex[players[i]] = static_cast<int>(i) + 1;
        }
    }

    std::map<int, std::vector<int>> pawnsByTile;
    for (size_t i = 0; i < players.size(); ++i) {
        Player* p = players[i];
        if (p == nullptr)
            continue;
        pawnsByTile[p->getPosition()].push_back(static_cast<int>(i) + 1);
    }

    auto ownerLineForTile = [&](Tile* tile) {
        std::string ownerText;
        if (PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile)) {
            Property* prop = propTile->getProperty();
            if (prop != nullptr && prop->getOwner() != nullptr) {
                auto ownerIt = playerIndex.find(prop->getOwner());
                if (ownerIt != playerIndex.end()) {
                    ownerText = "P" + std::to_string(ownerIt->second);
                }

                if (StreetProperty* street =
                        dynamic_cast<StreetProperty*>(prop)) {
                    if (street->hasHotel()) {
                        ownerText += " *";
                    } else {
                        const int level = street->getBuildingCount();
                        if (level > 0) {
                            ownerText +=
                                " " + std::string(static_cast<size_t>(
                                                      std::min(level, 4)),
                                                  '^');
                            if (level > 4) {
                                ownerText += "+";
                            }
                        }
                    }
                }

                if (prop->getStatus() == PropertyStatus::MORTGAGED) {
                    if (!ownerText.empty())
                        ownerText += " ";
                    ownerText += "[M]";
                }
            }
        }

        auto pawnIt = pawnsByTile.find(tile != nullptr ? tile->getIndex() : -1);
        if (pawnIt != pawnsByTile.end()) {
            std::ostringstream pawnStream;
            pawnStream << "(";
            for (size_t i = 0; i < pawnIt->second.size(); ++i) {
                if (i > 0)
                    pawnStream << ",";
                pawnStream << pawnIt->second[i];
            }
            pawnStream << ")";

            if (!ownerText.empty())
                ownerText += " ";
            ownerText += pawnStream.str();
        }

        return ownerText;
    };

    auto cellForIndex = [&](int idx) {
        Tile* tile = ctx.board.getTile(idx);
        if (tile == nullptr) {
            return CellContent{"[??] ---", "[??] ---", ""};
        }

        const auto codeAndAnsi = tileColorCodeAndAnsi(tile);
        const std::string plain =
            "[" + codeAndAnsi.first + "] " + tile->getCode();
        const std::string rendered = codeAndAnsi.second + plain + "\033[0m";
        return CellContent{plain, rendered, ownerLineForTile(tile)};
    };

    std::vector<int> topRow;
    for (int i = 20; i <= 30; ++i)
        topRow.push_back(i);
    std::vector<int> bottomRow;
    for (int i = 10; i >= 0; --i)
        bottomRow.push_back(i);
    std::vector<int> leftCol;
    for (int i = 19; i >= 11; --i)
        leftCol.push_back(i);
    std::vector<int> rightCol;
    for (int i = 31; i <= 39; ++i)
        rightCol.push_back(i);

    std::vector<CellContent> perimeterCells;
    for (int idx : topRow)
        perimeterCells.push_back(cellForIndex(idx));
    for (int idx : bottomRow)
        perimeterCells.push_back(cellForIndex(idx));
    for (int idx : leftCol)
        perimeterCells.push_back(cellForIndex(idx));
    for (int idx : rightCol)
        perimeterCells.push_back(cellForIndex(idx));

    size_t cellWidth = 10;
    for (const CellContent& cell : perimeterCells) {
        cellWidth = std::max(cellWidth, cell.line1Plain.size());
        cellWidth = std::max(cellWidth, cell.line2.size());
    }

    const TurnManager& tm = ctx.gameEngine.getTurnManager();
    const int currentTurnHuman = tm.getCurrentTurn() + 1;
    const int maxTurn = tm.getMaxTurn();
    const std::string turnText =
        maxTurn > 0 ? ("TURN " + std::to_string(currentTurnHuman) + " / " +
                       std::to_string(maxTurn))
                    : ("TURN " + std::to_string(currentTurnHuman) + " / INF");

	std::vector<std::string> centerLines = {
		"||          NIMONSPOLI          ||",
		"",
		turnText,
		"LEGENDA KEPEMILIKAN & STATUS",
		"P1-P4 : Properti milik pemain",
		"^     : Rumah",
		"*     : Hotel",
		"(1)   : Bidak pemain",
		"KODE WARNA:",
		"[CK]=Coklat  [MR]=Merah",
		"[BM]=Biru Muda [KN]=Kuning",
		"[PK]=Pink    [HJ]=Hijau",
		"[OR]=Orange  [BT]=Biru Tua",
		"[DF]=Aksi    [AB]=Utilitas"
	};

    size_t centerWidth = 34;
    for (const std::string& line : centerLines) {
        centerWidth = std::max(centerWidth, line.size());
    }

    // Keep middle rows aligned with the full top/bottom board width (11 cells).
    // Top row width: 1 + 11 * (cellWidth + 3)
    // Middle row width with left+center+right blocks: 2 * cellWidth +
    // centerWidth + 10 Solve for centerWidth to place the right block exactly
    // at the board edge.
    const size_t alignedCenterWidth = (9 * cellWidth) + 24;
    centerWidth = std::max(centerWidth, alignedCenterWidth);

    auto printRowBorder = [&](size_t cellCount) {
        out << "+";
        for (size_t i = 0; i < cellCount; ++i) {
            out << std::string(cellWidth + 2, '-') << "+";
        }
        out << "\n";
    };

    auto printCellLine = [&](const std::vector<CellContent>& rowCells,
                             int lineNo) {
        out << "|";
        for (const CellContent& cell : rowCells) {
            const std::string& content =
                (lineNo == 1) ? cell.line1Rendered : cell.line2;
            const std::string& plain =
                (lineNo == 1) ? cell.line1Plain : cell.line2;
            out << " " << content << std::string(cellWidth - plain.size(), ' ')
                << " |";
        }
        out << "\n";
    };

    std::vector<CellContent> topCells;
    for (int idx : topRow)
        topCells.push_back(cellForIndex(idx));
    printRowBorder(topCells.size());
    printCellLine(topCells, 1);
    printCellLine(topCells, 2);
    printRowBorder(topCells.size());

    for (size_t r = 0; r < leftCol.size(); ++r) {
        const CellContent leftCell = cellForIndex(leftCol[r]);
        const CellContent rightCell = cellForIndex(rightCol[r]);
        const std::string center = r < centerLines.size() ? centerLines[r] : "";

        out << "| " << leftCell.line1Rendered
            << std::string(cellWidth - leftCell.line1Plain.size(), ' ') << " | "
            << padRight(center, centerWidth) << " | " << rightCell.line1Rendered
            << std::string(cellWidth - rightCell.line1Plain.size(), ' ')
            << " |\n";

        out << "| " << leftCell.line2
            << std::string(cellWidth - leftCell.line2.size(), ' ') << " | "
            << std::string(centerWidth, ' ') << " | " << rightCell.line2
            << std::string(cellWidth - rightCell.line2.size(), ' ') << " |\n";

        out << "+" << std::string(cellWidth + 2, '-') << "+"
            << std::string(centerWidth + 2, '-') << "+"
            << std::string(cellWidth + 2, '-') << "+\n";
    }

    std::vector<CellContent> bottomCells;
    for (int idx : bottomRow)
        bottomCells.push_back(cellForIndex(idx));
    printCellLine(bottomCells, 1);
    printCellLine(bottomCells, 2);
    printRowBorder(bottomCells.size());
    out << "\n";
}