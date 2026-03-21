#include "TileMap.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "json.hpp" // nlohmann/json

using json = nlohmann::json;

TileMap::TileMap(const std::string &jsonPath) {
    // Read the file
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("TileMap: cannot open " + jsonPath);
    
    // Parse the JSON file directly into a json object
    json j;
    try {
        file >> j;
    } catch (json::parse_error& e) {
        throw std::runtime_error("TileMap JSON parse error: " + std::string(e.what()));
    }

    // Load background image
    std::string bgPath = j.value("background_path", "");
    if (bgPath.empty()) {
        std::cerr << "Warning: TileMap JSON missing background_path or empty\n";
    }

    const std::string resPrefix = "Resources";
    std::string absPath;
    if (bgPath.substr(0, resPrefix.size()) == resPrefix)
        absPath = std::string(RESOURCE_DIR) + bgPath.substr(resPrefix.size());
    else
        absPath = std::string(RESOURCE_DIR) + "/" + bgPath;

    auto bgImage = std::make_shared<Util::Image>(absPath);
    m_Background.SetDrawable(bgImage);
    m_Background.SetZIndex(-10.0f);
    m_Background.m_Transform.translation = {0.0f, 0.0f};
    m_Background.m_Transform.scale = {3.0f, 3.0f};

    // Load grid info
    if (j.contains("grid") && j["grid"].is_object()) {
        auto& gridObj = j["grid"];
        m_GridWidth  = gridObj.value("width", 0);
        m_GridHeight = gridObj.value("height", 0);
        m_TileSize   = gridObj.value("tile_size", 0);
    } else {
        std::cerr << "Warning: TileMap JSON missing valid 'grid' object\n";
    }

    // Load layer data
    if (j.contains("layers") && j["layers"].is_array() && !j["layers"].empty()) {
        auto& firstLayer = j["layers"][0];
        if (firstLayer.contains("data") && firstLayer["data"].is_array()) {
            m_Grid = firstLayer["data"].get<std::vector<int>>();
        } else {
            std::cerr << "Warning: First layer missing valid 'data' array\n";
        }
    } else {
        std::cerr << "Warning: TileMap JSON missing valid 'layers' array\n";
    }

    const int expected = m_GridWidth * m_GridHeight;
    m_Grid.resize(static_cast<size_t>(expected), -1);

    if (j.contains("connection")) {
        m_Connections.up = j["connection"].value("up", -1);
        m_Connections.right = j["connection"].value("right", -1);
        m_Connections.down = j["connection"].value("down", -1);
        m_Connections.left = j["connection"].value("left", -1);
    } else if (j.contains("connection_id")) {
        m_Connections.up = j["connection_id"].value("up", -1);
        m_Connections.right = j["connection_id"].value("right", -1);
        m_Connections.down = j["connection_id"].value("down", -1);
        m_Connections.left = j["connection_id"].value("left", -1);
    }
}

void TileMap::Draw() {
    m_Background.Draw();
}

TileMap::TileType TileMap::GetTileType(int col, int row) const {
    if (col < 0 || col >= m_GridWidth || row < 0 || row >= m_GridHeight)
        return TileType::Wall;
    return static_cast<TileType>(m_Grid[static_cast<size_t>(row * m_GridWidth + col)]);
}

glm::vec2 TileMap::GridToScreen(int col, int row) const {
    float scaleX = m_Background.m_Transform.scale.x;
    float scaleY = m_Background.m_Transform.scale.y;
    float sx = (col - m_GridWidth  / 2.0f + 0.5f) * (m_TileSize * scaleX);
    float sy = -(row - m_GridHeight / 2.0f + 0.5f) * (m_TileSize * scaleY);
    return {sx, sy};
}

glm::ivec2 TileMap::ScreenToGrid(const glm::vec2& screenPos) const {
    if (m_TileSize <= 0) return {0, 0};
    float scaleX = m_Background.m_Transform.scale.x;
    float scaleY = m_Background.m_Transform.scale.y;
    float scaledTileWidth = m_TileSize * scaleX;
    float scaledTileHeight = m_TileSize * scaleY;

    int col = static_cast<int>(std::floor(screenPos.x / scaledTileWidth + m_GridWidth / 2.0f));
    int row = static_cast<int>(std::floor(-screenPos.y / scaledTileHeight + m_GridHeight / 2.0f));
    return {col, row};
}
