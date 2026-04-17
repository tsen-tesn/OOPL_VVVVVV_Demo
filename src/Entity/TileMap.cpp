#include "TileMap.hpp"
#include <cmath>

TileMap::TileMap(int gridWidth, int gridHeight, int tileSize, const std::vector<int>& gridData)
    : m_GridWidth(gridWidth), m_GridHeight(gridHeight), m_TileSize(tileSize), m_Grid(gridData) {
}

TileMap::TileType TileMap::GetTileType(int col, int row) const {
    if (col < 0 || col >= m_GridWidth || row < 0 || row >= m_GridHeight)
        return TileType::Wall;
    return static_cast<TileType>(m_Grid[static_cast<size_t>(row * m_GridWidth + col)]);
}

glm::vec2 TileMap::GridToScreen(float col, float row) const {
    float scaleX = 3.0f;
    float scaleY = 3.0f;

    float sx = (col - m_GridWidth  / 2.0f + 0.5f) * (m_TileSize * scaleX);
    float sy = -(row - m_GridHeight / 2.0f + 0.5f) * (m_TileSize * scaleY);

    return {sx, sy};
}

glm::ivec2 TileMap::ScreenToGrid(const glm::vec2& screenPos) const {
    float scaleX = 3.0f;
    float scaleY = 3.0f;
    float scaledTileWidth  = m_TileSize * scaleX;
    float scaledTileHeight = m_TileSize * scaleY;

    int col = static_cast<int>(std::floor(screenPos.x / scaledTileWidth + m_GridWidth / 2.0f));
    int row = static_cast<int>(std::floor(-screenPos.y / scaledTileHeight + m_GridHeight / 2.0f));

    return {col, row};
}
