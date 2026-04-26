#include "gui/modules/GuiLayoutModule.hpp"

#include <algorithm>
#include <cctype>

namespace {
constexpr int kTilesPerSide = 10;
constexpr int kTotalTiles = 40;

std::string ToLowerLocal(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
    return value;
}
} // namespace

void GuiLayoutModule::Recalculate(Rectangle& boardBounds,
                                  Rectangle& centerArea,
                                  Rectangle& rightPanel,
                                  Rectangle& bottomPanel,
                                  float& tileSize,
                                  const int screenWidth,
                                  const int screenHeight,
                                  const int boardMargin,
                                  const int rightPanelWidth,
                                  const int bottomPanelHeight) {
    // Calculate board size - use available height minus margins
    const float usableHeight =
        static_cast<float>(screenHeight - boardMargin * 2);

    // Board is square, calculate size
    float boardSize = usableHeight;
    boardSize = std::max(boardSize, 12.0F * 26.0F);

    tileSize = boardSize / 12.0F;

    // Center the board horizontally, leave space on right for panel
    float totalWidth = boardSize + boardMargin + rightPanelWidth;
    float startX = (static_cast<float>(screenWidth) - totalWidth) * 0.5f;
    if (startX < boardMargin)
        startX = boardMargin;

    boardBounds = Rectangle{
        startX,
        static_cast<float>(boardMargin),
        tileSize * 12.0F,
        tileSize * 12.0F,
    };

    centerArea = Rectangle{boardBounds.x + tileSize,
                           boardBounds.y + tileSize,
                           boardBounds.width - tileSize * 2.0F,
                           boardBounds.height - tileSize * 2.0F};

    // Right panel at far right, vertically centered with board
    rightPanel = Rectangle{boardBounds.x + boardBounds.width + boardMargin,
                           boardBounds.y,
                           static_cast<float>(rightPanelWidth),
                           boardBounds.height};

    // Bottom panel below board, same width as board
    bottomPanel = Rectangle{boardBounds.x,
                            boardBounds.y + boardBounds.height + boardMargin,
                            boardBounds.width,
                            static_cast<float>(bottomPanelHeight)};
}

GUIRenderer::Square GuiLayoutModule::TileSquare(const int index,
                                                const float tileSize,
                                                const Rectangle& boardBounds) {
    if (index < 1 || index > kTotalTiles || tileSize <= 0.0F) {
        return {};
    }

    const float left = boardBounds.x;
    const float top = boardBounds.y;
    const float right = boardBounds.x + boardBounds.width;
    const float bottom = boardBounds.y + boardBounds.height;

    const int segment = index - 1;

    if (segment < kTilesPerSide) {
        return GUIRenderer::Square{
            right - tileSize * (segment + 1), bottom - tileSize, tileSize, 0};
    }

    if (segment < 2 * kTilesPerSide) {
        const int local = segment - kTilesPerSide;
        return GUIRenderer::Square{
            left, bottom - tileSize * (local + 2), tileSize, 1};
    }

    if (segment < 3 * kTilesPerSide) {
        const int local = segment - 2 * kTilesPerSide;
        return GUIRenderer::Square{
            left + tileSize * (local + 1), top, tileSize, 2};
    }

    const int local = segment - 3 * kTilesPerSide;
    return GUIRenderer::Square{
        right - tileSize, top + tileSize * (local + 1), tileSize, 3};
}

int GuiLayoutModule::ResolveTileIndex(
    const std::unordered_map<std::string, int>& tileIndexByCode,
    const std::string& code) {
    if (code.empty())
        return -1;

    const std::string lowered = ToLowerLocal(code);
    const auto fromMap = tileIndexByCode.find(lowered);
    if (fromMap != tileIndexByCode.end()) {
        return fromMap->second;
    }

    std::string digits;
    for (const char ch : code) {
        if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
            digits.push_back(ch);
        }
    }

    if (!digits.empty()) {
        const int parsed = std::stoi(digits);
        if (parsed >= 1 && parsed <= kTotalTiles) {
            return parsed;
        }
    }

    return -1;
}