#ifndef TILE_MAP_HPP
#define TILE_MAP_HPP

#include <memory>
#include <string>
#include <vector>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"


class TileMap {
public:
    enum class TileType {
        Wall = 0,
        Path = 1 
    };

    TileMap(int gridWidth, int gridHeight, int tileSize, const std::vector<int>& gridData);

    TileType GetTileType(int col, int row) const;

    int GetGridWidth()   const { return m_GridWidth; }
    int GetGridHeight()  const { return m_GridHeight; }
    int GetTileSize()    const { return m_TileSize; }

    glm::vec2 GridToScreen(float col, float row) const;
    glm::ivec2 ScreenToGrid(const glm::vec2& screenPos) const;

private:
    int m_GridWidth;
    int m_GridHeight;
    int m_TileSize;

    std::vector<int> m_Grid;
};

#endif
