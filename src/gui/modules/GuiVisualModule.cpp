#include "gui/modules/GuiVisualModule.hpp"

#include <algorithm>
#include <array>
#include <cctype>

namespace {
constexpr int kTilesPerSide = 10;
constexpr int kCornerCount = 4;

std::string ToLowerLocal(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
    return value;
}

bool ContainsInsensitiveLocal(const std::string& value,
                              const std::string& needle) {
    return ToLowerLocal(value).find(ToLowerLocal(needle)) != std::string::npos;
}
} // namespace

Color GuiVisualModule::GetColorGroupForTile(const int index) {
    static const std::array<Color, 10> palette = {
        Color{151, 95, 64, 255},
        Color{129, 185, 235, 255},
        Color{219, 113, 164, 255},
        Color{242, 164, 75, 255},
        Color{221, 75, 65, 255},
        Color{234, 205, 82, 255},
        Color{95, 184, 76, 255},
        Color{58, 150, 142, 255},
        Color{56, 106, 194, 255},
        Color{160, 93, 165, 255},
    };

    const int side = ((index - 1) / (kTilesPerSide + 1)) % kCornerCount;
    if ((index == 1) || (index == 11) || (index == 21) || (index == 31)) {
        return Color{205, 197, 178, 255};
    }

    const int colorIdx =
        (side * 2 + (index % 2)) % static_cast<int>(palette.size());
    return palette[static_cast<std::size_t>(colorIdx)];
}

std::string GuiVisualModule::ResolveTileTextureKey(
    const std::string& code,
    const std::string* propertyType,
    const std::unordered_map<std::string, std::string>& tileTextureByCode) {
    const std::string loweredCode = ToLowerLocal(code);

    const auto visualMapIt = tileTextureByCode.find(loweredCode);
    if (visualMapIt != tileTextureByCode.end()) {
        return visualMapIt->second;
    }

    if (ContainsInsensitiveLocal(loweredCode, "go") ||
        ContainsInsensitiveLocal(loweredCode, "mulai")) {
        return "tile-go";
    }
    if (ContainsInsensitiveLocal(loweredCode, "stasiun") ||
        ContainsInsensitiveLocal(loweredCode, "railroad") ||
        ContainsInsensitiveLocal(loweredCode, "rr")) {
        return "tile-stasiun";
    }
    if (ContainsInsensitiveLocal(loweredCode, "pln")) {
        return "tile-pln";
    }
    if (ContainsInsensitiveLocal(loweredCode, "pam")) {
        return "tile-pam";
    }
    if (ContainsInsensitiveLocal(loweredCode, "festival")) {
        return "tile-festival";
    }
    if (ContainsInsensitiveLocal(loweredCode, "kesempatan") ||
        ContainsInsensitiveLocal(loweredCode, "chance") ||
        ContainsInsensitiveLocal(loweredCode, "dana") ||
        ContainsInsensitiveLocal(loweredCode, "umum") ||
        ContainsInsensitiveLocal(loweredCode, "community")) {
        return "tile-kesempatan";
    }

    if (propertyType != nullptr) {
        if (ContainsInsensitiveLocal(*propertyType, "railroad")) {
            return "tile-stasiun";
        }
        if (ContainsInsensitiveLocal(*propertyType, "utility") &&
            ContainsInsensitiveLocal(loweredCode, "pln")) {
            return "tile-pln";
        }
        if (ContainsInsensitiveLocal(*propertyType, "utility") &&
            ContainsInsensitiveLocal(loweredCode, "pam")) {
            return "tile-pam";
        }
    }

    return "";
}

std::string
GuiVisualModule::ResolveHandCardTextureKey(const std::string& cardType) {
    const std::string type = ToLowerLocal(cardType);
    if (ContainsInsensitiveLocal(type, "move")) {
        return "card-move";
    }
    if (ContainsInsensitiveLocal(type, "discount")) {
        return "card-discount";
    }
    if (ContainsInsensitiveLocal(type, "shield")) {
        return "card-shield";
    }
    if (ContainsInsensitiveLocal(type, "teleport")) {
        return "card-teleport";
    }
    if (ContainsInsensitiveLocal(type, "lasso")) {
        return "card-lasso";
    }
    if (ContainsInsensitiveLocal(type, "demolition")) {
        return "card-demolition";
    }
    return "";
}
