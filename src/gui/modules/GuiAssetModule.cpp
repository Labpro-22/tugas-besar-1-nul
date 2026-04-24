#include "gui/modules/GuiAssetModule.hpp"

#include <filesystem>
#include <utility>
#include <vector>

#if NIMONSPOLI_HAS_RAYLIB
#include <raylib.h>
#endif

namespace {
using AssetRegistry = std::vector<std::pair<std::string, std::string>>;

std::string JoinPath(const std::string& root, const std::string& filename) {
    if (root.empty()) {
        return filename;
    }
    if (root.back() == '/') {
        return root + filename;
    }
    return root + "/" + filename;
}

const AssetRegistry& GetAssetRegistry() {
    static const AssetRegistry registry = {
        {"dice1", "dice1.png"},
        {"dice2", "dice2.png"},
        {"dice3", "dice3.png"},
        {"dice4", "dice4.png"},
        {"dice5", "dice5.png"},
        {"dice6", "dice6.png"},

        {"player1", "player1.png"},
        {"player2", "player2.png"},
        {"player3", "player3.png"},
        {"player4", "player4.png"},

        {"tile-go", "go.png"},
        {"tile-stasiun", "stasiun.png"},
        {"tile-pln", "pln.png"},
        {"tile-pam", "pam.png"},
        {"tile-festival", "festival.png"},
        {"tile-kesempatan", "kesempatan.png"},

        {"deck-dana-umum", "kartu-dana-umum.png"},
        {"deck-kesempatan", "kartu-kesempatan.png"},

        {"card-move", "movecard.png"},
        {"card-discount", "discountcard.png"},
        {"card-shield", "shieldcard.png"},
        {"card-teleport", "teleportcard.png"},
        {"card-lasso", "lassocard.png"},
        {"card-demolition", "demolitioncard.png"},

        {"building-house", "house.png"},
        {"building-hotel", "hotel.png"},

        {"overlay-mortgage", "mortgage-lock.png"},
        {"overlay-festival", "festival-effect.png"},
        {"nimonspoli", "nimonspoli.png"},
    };
    return registry;
}
} // namespace

bool GuiAssetModule::LoadAll(GUIRenderer::AssetMap& textures,
                             bool& assetsLoaded,
                             const std::string& assetRoot) {
    UnloadAll(textures, assetsLoaded);

    bool allLoaded = true;
    for (const auto& asset : GetAssetRegistry()) {
        allLoaded =
            LoadSingle(textures, assetRoot, asset.first, asset.second) &&
            allLoaded;
    }

    assetsLoaded = allLoaded;
    return allLoaded;
}

bool GuiAssetModule::LoadSingle(GUIRenderer::AssetMap& textures,
                                const std::string& assetRoot,
                                const std::string& key,
                                const std::string& fileName) {
#if NIMONSPOLI_HAS_RAYLIB
    const std::string fullPath = JoinPath(assetRoot, fileName);
    if (!std::filesystem::exists(fullPath)) {
        return false;
    }

    Texture2D texture = LoadTexture(fullPath.c_str());
    if (texture.id == 0) {
        return false;
    }

    textures[key] = texture;
    return true;
#else
    (void) textures;
    (void) assetRoot;
    (void) key;
    (void) fileName;
    return false;
#endif
}

void GuiAssetModule::UnloadAll(GUIRenderer::AssetMap& textures,
                               bool& assetsLoaded) {
#if NIMONSPOLI_HAS_RAYLIB
    for (auto& entry : textures) {
        if (entry.second.id != 0) {
            UnloadTexture(entry.second);
        }
    }
#endif
    textures.clear();
    assetsLoaded = false;
}

bool GuiAssetModule::Has(const GUIRenderer::AssetMap& textures,
                         const std::string& key) {
    const auto it = textures.find(key);
    return it != textures.end() && it->second.id != 0;
}

Texture2D GuiAssetModule::GetOrEmpty(const GUIRenderer::AssetMap& textures,
                                     const std::string& key) {
    const auto it = textures.find(key);
    if (it == textures.end()) {
        return Texture2D{};
    }
    return it->second;
}
