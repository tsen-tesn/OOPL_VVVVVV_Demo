#include "Loadlevel.hpp"
#include "DisappearingPlatformGroup.hpp"
#include "MovingPlatform.hpp"
#include "MovingEnemy.hpp"
#include "Spike.hpp"
#include "CheckPoint.hpp"
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
                        float col = pos.value("col", 0.0f);
                        float row = pos.value("row", 0.0f);
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
                        
                        float startCol = path["start"].value("col", 0.0f);
                        float startRow = path["start"].value("row", 0.0f);
                        float endCol = path["end"].value("col", 0.0f);
                        float endRow = path["end"].value("row", 0.0f);

                        glm::vec2 startPos = m_TileMap->GridToScreen(startCol, startRow);
                        glm::vec2 endPos = m_TileMap->GridToScreen(endCol, endRow);

                        auto enemy = std::make_shared<MovingEnemy>(startPos, endPos, animPaths, scaleValue, speedValue);
                        enemy->SetZIndex(10.0f);
                        m_Hazards.push_back(enemy);
                    }
                }
            }

            else if (name == "DisappearingPlatformGroup") {
                std::vector<std::string> imagePaths;
                if (layer.contains("image_paths") && layer["image_paths"].is_array()) {
                    imagePaths = layer["image_paths"].get<std::vector<std::string>>();
                } else if (layer.contains("image_path")) {
                    imagePaths.push_back(layer["image_path"].get<std::string>());
                }
                
                if (imagePaths.empty()) continue;

                if (layer.contains("positions") && layer["positions"].is_array()) {
                    const auto& positionsArray = layer["positions"];
                    if (!positionsArray.empty() && positionsArray[0].is_array()) {
                        // 二維陣列格式：多個 group
                        for (const auto& groupPositions : positionsArray) {
                            if (!groupPositions.is_array()) continue;
                            std::vector<glm::vec2> positions;
                            for (const auto& pos : groupPositions) {
                                if (!pos.is_object()) continue;
                                float col = pos.value("col", 0.0f);
                                float row = pos.value("row", 0.0f);
                                glm::vec2 screenPos = m_TileMap->GridToScreen(col, row);
                                positions.push_back(screenPos);
                            }
                            if (positions.empty()) continue;
                            auto group = std::make_shared<DisappearingPlatformGroup>(positions, imagePaths, 3.0f);
                            m_Platforms.push_back(group);
                        }
                    } else {
                        // 一維陣列格式：單一 group
                        std::vector<glm::vec2> positions;
                        for (const auto& pos : positionsArray) {
                            if (!pos.is_object()) continue;
                            float col = pos.value("col", 0.0f);
                            float row = pos.value("row", 0.0f);
                            glm::vec2 screenPos = m_TileMap->GridToScreen(col, row);
                            positions.push_back(screenPos);
                        }
                        if (!positions.empty()) {
                            auto group = std::make_shared<DisappearingPlatformGroup>(positions, imagePaths, 3.0f);
                            m_Platforms.push_back(group);
                        }
                    }
                }
            }

            else if (name == "MovingPlatform") {
                std::vector<std::string> imagePaths;
                if (layer.contains("image_paths") && layer["image_paths"].is_array()) {
                    imagePaths = layer["image_paths"].get<std::vector<std::string>>();
                } else if (layer.contains("image_path")) {
                    imagePaths.push_back(layer["image_path"].get<std::string>());
                }
                
                if (imagePaths.empty()) continue;

                float scaleValue = layer.value("scale", 3.0f);
                float speedValue = layer.value("speed", 200.0f);

                if (layer.contains("positions") && layer["positions"].is_array()) {
                    const auto& positionsArray = layer["positions"];
                    size_t numPlatforms = std::min(imagePaths.size(), positionsArray.size());
                    for (size_t i = 0; i < numPlatforms; ++i) {
                        const auto& pos = positionsArray[i];
                        if (!pos.is_object() || !pos.contains("start") || !pos.contains("end")) continue;
                        
                        const auto& start = pos["start"];
                        const auto& end = pos["end"];
                        if (!start.is_object() || !end.is_object()) continue;
                        
                        float startCol = start.value("col", 0.0f);
                        float startRow = start.value("row", 0.0f);
                        float endCol = end.value("col", 0.0f);
                        float endRow = end.value("row", 0.0f);
                        
                        glm::vec2 startPos = m_TileMap->GridToScreen(startCol, startRow);
                        glm::vec2 endPos = m_TileMap->GridToScreen(endCol, endRow);
                        
                        auto platform = std::make_shared<MovingPlatform>(startPos, endPos, imagePaths[i], scaleValue, speedValue);
                        m_Platforms.push_back(platform);
                    }
                }
            }
        }
            // Handle CheckPoint
            else if (name == "CheckPoint") {
                std::string imagePath;
                std::string type = layer.value("type", "");
                if (type == "up") {
                    imagePath = "Resources/Trigger/trigger_5.png";
                } else if (type == "down") {
                    imagePath = "Resources/Trigger/trigger_4.png";
                }

                if (imagePath.empty()) continue;

                if(layer.contains("positions") && layer["positions"].is_array()) {
                    for (const auto& pos : layer["positions"]) {
                        float col = pos.value("col", 0.0f);
                        float row = pos.value("row", 0.0f);
                        glm::vec2 screenPos = m_TileMap->GridToScreen(col, row);
                        
                        auto checkPoint = std::make_shared<CheckPoint>(screenPos, imagePath);
                        checkPoint->SetZIndex(5.0f);
                        m_Trigger.push_back(checkPoint);
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
    for (auto& trigger : m_Trigger) {
        trigger->Draw();
    }
}
