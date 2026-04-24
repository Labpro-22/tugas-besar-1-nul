#pragma once

#include <string>
#include <unordered_map>

#include "gui/GuiRenderer.hpp"

class GuiLayoutModule {
  public:
    static void Recalculate(Rectangle& boardBounds,
                            Rectangle& centerArea,
                            Rectangle& rightPanel,
                            Rectangle& bottomPanel,
                            float& tileSize,
                            int screenWidth,
                            int screenHeight,
                            int boardMargin,
                            int rightPanelWidth,
                            int bottomPanelHeight);

    static GUIRenderer::Square
    TileSquare(int index, float tileSize, const Rectangle& boardBounds);

    static int ResolveTileIndex(
        const std::unordered_map<std::string, int>& tileIndexByCode,
        const std::string& code);
};
