#include "core/Command.hpp"

#include "core/TurnContext.hpp"

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
		out << "[WARN] Perintah kosong. Ketik HELP untuk daftar perintah.\n";
		return false;
	}

	const std::string commandName = argv(0);

	if (commandName == "LEMPAR_DADU") {
		out << "[INFO] Memproses perintah: LEMPAR_DADU\n";
		ctx.dice.roll();
		ctx.diceTotal = ctx.dice.getTotal();
		out << "[INFO] Total dadu: " << ctx.diceTotal << "\n";
		return true;
	}

	if (commandName == "HELP") {
		out << "[INFO] Daftar perintah:\n";
		out << "  - LEMPAR_DADU\n";
		out << "  - HELP\n";
		return true;
	}

	out << "[WARN] Perintah tidak dikenal: " << commandName << "\n";
	return false;
}
