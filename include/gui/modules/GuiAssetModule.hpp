#pragma once

#include <string>

#include "gui/GuiRenderer.hpp"

class GuiAssetModule {
  public:
    static bool LoadAll(GUIRenderer::AssetMap& textures,
                        bool& assetsLoaded,
                        const std::string& assetRoot);

    static bool LoadSingle(GUIRenderer::AssetMap& textures,
                           const std::string& assetRoot,
                           const std::string& key,
                           const std::string& fileName);

    static void UnloadAll(GUIRenderer::AssetMap& textures, bool& assetsLoaded);

    static bool Has(const GUIRenderer::AssetMap& textures,
                    const std::string& key);

    static Texture2D GetOrEmpty(const GUIRenderer::AssetMap& textures,
                                const std::string& key);
};
