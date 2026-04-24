#pragma once

#include <string>
#include <unordered_map>

#include "gui/GuiRenderer.hpp"

class GuiVisualModule {
  public:
    static Color GetColorGroupForTile(int index);

    static std::string ResolveTileTextureKey(
        const std::string& code,
        const std::string* propertyType,
        const std::unordered_map<std::string, std::string>& tileTextureByCode);

    static std::string ResolveHandCardTextureKey(const std::string& cardType);
};
