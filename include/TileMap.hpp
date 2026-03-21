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
        Wall = -1,
        Path = 1
    };

    struct Connections {
        int up = -1;
        int right = -1;
        int down = -1;
        int left = -1;
    };

    explicit TileMap(const std::string &jsonPath);

    void Draw();

    TileType GetTileType(int col, int row) const;

    int GetGridWidth()   const { return m_GridWidth; }
    int GetGridHeight()  const { return m_GridHeight; }
    int GetTileSize()    const { return m_TileSize; }
    Connections GetConnections() const { return m_Connections; }

    glm::vec2 GridToScreen(int col, int row) const;
    glm::ivec2 ScreenToGrid(const glm::vec2& screenPos) const;

private:
    // 45 * 30, 8 pixel per tile
    int m_GridWidth  = 45;
    int m_GridHeight = 30;
    int m_TileSize   = 8;

    std::vector<int>  m_Grid;
    Util::GameObject  m_Background;
    Connections       m_Connections;
};

#endif
