#include "Loadlevel.hpp"
#include "MovingEnemy.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

#include <json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

LoadLevel::LoadLevel(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("LoadLevel: cannot open " + jsonPath);
    
    json j;
    try {
        file >> j;
    } catch (json::parse_error& e) {
        throw std::runtime_error("LoadLevel JSON parse error: " + std::string(e.what()));
    }

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

    std::string bgPath = j.value("background_path", "");
    if (!bgPath.empty()) {
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
    }

    int gridWidth = 45;
    int gridHeight = 30;
    int tileSize = 8;
    if (j.contains("grid") && j["grid"].is_object()) {
        auto& gridObj = j["grid"];
        gridWidth  = gridObj.value("width", 45);
        gridHeight = gridObj.value("height", 30);
        tileSize   = gridObj.value("tile_size", 8);
    }

    std::vector<int> gridData;
    if (j.contains("layers") && j["layers"].is_array()) {
        for (const auto& layer : j["layers"]) {
            if (layer.contains("data") && layer["data"].is_array()) {
                gridData = layer["data"].get<std::vector<int>>();
                break;
            }
        }
    }
    const int expected = gridWidth * gridHeight;
    gridData.resize(static_cast<size_t>(expected), -1);

    m_TileMap = std::make_shared<TileMap>(gridWidth, gridHeight, tileSize, gridData);

    if (j.contains("layers") && j["layers"].is_array()) {
        for (const auto& layer : j["layers"]) {
            std::string name = layer.value("name", "");
            
            // Handle Spikes
            if (name == "Spikes") {
                std::string imagePath = layer.value("image_path", "");
                if (imagePath.empty()) continue;

                if (layer.contains("positions") && layer["positions"].is_array()) {
                    for (const auto& pos : layer["positions"]) {
                        int col = pos.value("col", 0);
                        int row = pos.value("row", 0);
                        glm::vec2 screenPos = m_TileMap->GridToScreen(col, row);
                        
                        auto spike = std::make_shared<Spike>(screenPos, imagePath);
                        spike->SetZIndex(5.0f);
                        m_Hazards.push_back(spike);
                    }
                }
            } 
            // Handle MovingEnemy
            else if (name == "Moving_enemy") {
                std::vector<std::string> animPaths;
                if (layer.contains("image_paths") && layer["image_paths"].is_array()) {
                    animPaths = layer["image_paths"].get<std::vector<std::string>>();
                } else if (layer.contains("image_path")) {
                    animPaths.push_back(layer["image_path"].get<std::string>());
                }
                
                if (animPaths.empty()) continue;

                float scaleValue = layer.value("scale", 2.0f);

                float speedValue = layer.value("speed", 100.0f);

                if (layer.contains("moving_paths") && layer["moving_paths"].is_array()) {
                    for (const auto& path : layer["moving_paths"]) {
                        if (!path.contains("start") || !path.contains("end")) continue;
                        
                        int startCol = path["start"].value("col", 0);
                        int startRow = path["start"].value("row", 0);
                        int endCol = path["end"].value("col", 0);
                        int endRow = path["end"].value("row", 0);

                        glm::vec2 startPos = m_TileMap->GridToScreen(startCol, startRow);
                        glm::vec2 endPos = m_TileMap->GridToScreen(endCol, endRow);

                        auto enemy = std::make_shared<MovingEnemy>(startPos, endPos, animPaths, scaleValue, speedValue);
                        enemy->SetZIndex(10.0f);
                        m_Hazards.push_back(enemy);
                    }
                }
            }
        }
    }
}

void LoadLevel::Draw() {
    m_Background.Draw();
    for (auto& hazard : m_Hazards) {
        hazard->Draw();
    }
}
