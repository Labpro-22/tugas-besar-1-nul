#include "gui/modules/GuiVisualModule.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <map>

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

const std::map<std::string, Color>& GetStreetColorMap() {
    static const std::map<std::string, Color> colorMap = {
        {"coklat",      Color{151, 95, 64, 255}},
        {"biru muda",   Color{129, 185, 235, 255}},
        {"merah muda",  Color{219, 113, 164, 255}},
        {"orange",      Color{242, 164, 75, 255}},
        {"merah",       Color{221, 75, 65, 255}},
        {"kuning",      Color{234, 205, 82, 255}},
        {"hijau",       Color{95, 184, 76, 255}},
        {"biru tua",    Color{56, 106, 194, 255}},
    };
    return colorMap;
}

Color GetStreetColorByGroup(const std::string& colorGroup) {
    std::string lower = ToLowerLocal(colorGroup);
    std::replace(lower.begin(), lower.end(), '_', ' ');
    const auto& map = GetStreetColorMap();
    auto it = map.find(lower);
    if (it != map.end()) {
        return it->second;
    }
    return Color{160, 93, 165, 255};
}
} // namespace

Color GuiVisualModule::GetColorGroupForTile(int index, const PropertyState* property) {
    if ((index == 1) || (index == 11) || (index == 21) || (index == 31)) {
        return Color{205, 197, 178, 255};
    }

    if (property != nullptr && !property->colorGroup.empty()) {
        return GetStreetColorByGroup(property->colorGroup);
    }

    return Color{160, 93, 165, 255};
}

const std::string& GuiVisualModule::GetColorGroupFromProperty(const PropertyState* property) {
    static const std::string empty;
    if (property != nullptr) {
        return property->colorGroup;
    }
    return empty;
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
    if (ContainsInsensitiveLocal(loweredCode, "pen") ||
        ContainsInsensitiveLocal(loweredCode, "jail") ||
        ContainsInsensitiveLocal(loweredCode, "penjara")) {
        return "tile-jail";
    }
    if (ContainsInsensitiveLocal(loweredCode, "bbp") ||
        ContainsInsensitiveLocal(loweredCode, "free") ||
        ContainsInsensitiveLocal(loweredCode, "parkir") ||
        ContainsInsensitiveLocal(loweredCode, "bebas")) {
        return "tile-free-parking";
    }
    if (ContainsInsensitiveLocal(loweredCode, "ppj") ||
        ContainsInsensitiveLocal(loweredCode, "gotojail") ||
        ContainsInsensitiveLocal(loweredCode, "pergi ke penjara")) {
        return "tile-go-to-jail";
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
    if (ContainsInsensitiveLocal(loweredCode, "fes") ||
        ContainsInsensitiveLocal(loweredCode, "festival")) {
        return "tile-festival";
    }
    if (ContainsInsensitiveLocal(loweredCode, "kesempatan") ||
        ContainsInsensitiveLocal(loweredCode, "chance") ||
        ContainsInsensitiveLocal(loweredCode, "ksp")) {
        return "tile-kesempatan";
    }
    if (ContainsInsensitiveLocal(loweredCode, "dnu") ||
        ContainsInsensitiveLocal(loweredCode, "dana") ||
        ContainsInsensitiveLocal(loweredCode, "umum") ||
        ContainsInsensitiveLocal(loweredCode, "community")) {
        return "tile-kesempatan";
    }
    if (ContainsInsensitiveLocal(loweredCode, "pph") ||
        ContainsInsensitiveLocal(loweredCode, "pbm") ||
        ContainsInsensitiveLocal(loweredCode, "pajak") ||
        ContainsInsensitiveLocal(loweredCode, "tax")) {
        return "tile-tax";
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