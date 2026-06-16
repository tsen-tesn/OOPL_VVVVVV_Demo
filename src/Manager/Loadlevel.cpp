#include "Manager/Loadlevel.hpp"
#include "Entity/Platform/DisappearingPlatformGroup.hpp"
#include "Entity/Platform/MovingPlatform.hpp"
#include "Entity/Hazard/MovingEnemy.hpp"
#include "Entity/Hazard/Spike.hpp"
#include "Entity/Trigger/CheckPoint.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace {
json ReadLevelJson(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("LoadLevel: cannot open " + jsonPath);

    json levelJson;
    try {
        file >> levelJson;
    } catch (json::parse_error& e) {
        throw std::runtime_error("LoadLevel JSON parse error: " + std::string(e.what()));
    }

    return levelJson;
}

std::vector<std::string> GetImagePaths(const json& layer) {
    std::vector<std::string> imagePaths;
    if (layer.contains("image_paths") && layer["image_paths"].is_array()) {
        imagePaths = layer["image_paths"].get<std::vector<std::string>>();
    } else if (layer.contains("image_path")) {
        imagePaths.push_back(layer["image_path"].get<std::string>());
    }
    return imagePaths;
}
}

LoadLevel::LoadLevel(const std::string& jsonPath)
    : LoadLevel(ReadLevelJson(jsonPath)) {}

LoadLevel::LoadLevel(const json& levelJson) {
    LoadConnections(levelJson);
    LoadBackground(levelJson);
    LoadTileMap(levelJson);
    LoadLayers(levelJson);
}

void LoadLevel::LoadConnections(const json& levelJson) {
    if (levelJson.contains("connection")) {
        m_Connections.up = levelJson["connection"].value("up", -1);
        m_Connections.right = levelJson["connection"].value("right", -1);
        m_Connections.down = levelJson["connection"].value("down", -1);
        m_Connections.left = levelJson["connection"].value("left", -1);
    } else if (levelJson.contains("connection_id")) {
        m_Connections.up = levelJson["connection_id"].value("up", -1);
        m_Connections.right = levelJson["connection_id"].value("right", -1);
        m_Connections.down = levelJson["connection_id"].value("down", -1);
        m_Connections.left = levelJson["connection_id"].value("left", -1);
    }
}

void LoadLevel::LoadBackground(const json& levelJson) {
    std::string bgPath = levelJson.value("background_path", "");
    if (bgPath.empty()) {
        return;
    }

    const std::string resPrefix = "Resources";
    std::string absPath;
    if (bgPath.substr(0, resPrefix.size()) == resPrefix) {
        absPath = std::string(RESOURCE_DIR) + bgPath.substr(resPrefix.size());
    } else {
        absPath = std::string(RESOURCE_DIR) + "/" + bgPath;
    }

    auto bgImage = std::make_shared<Util::Image>(absPath);
    m_Background.SetDrawable(bgImage);
    m_Background.SetZIndex(-10.0f);
    m_Background.m_Transform.translation = {0.0f, 0.0f};
    m_Background.m_Transform.scale = {3.0f, 3.0f};
}

void LoadLevel::LoadTileMap(const json& levelJson) {
    int gridWidth = 45;
    int gridHeight = 30;
    int tileSize = 8;
    if (levelJson.contains("grid") && levelJson["grid"].is_object()) {
        const auto& gridObj = levelJson["grid"];
        gridWidth = gridObj.value("width", 45);
        gridHeight = gridObj.value("height", 30);
        tileSize = gridObj.value("tile_size", 8);
    }

    std::vector<int> gridData;
    if (levelJson.contains("layers") && levelJson["layers"].is_array()) {
        for (const auto& layer : levelJson["layers"]) {
            if (layer.contains("data") && layer["data"].is_array()) {
                gridData = layer["data"].get<std::vector<int>>();
                break;
            }
        }
    }

    const int expected = gridWidth * gridHeight;
    gridData.resize(static_cast<size_t>(expected), -1);
    m_TileMap = std::make_shared<TileMap>(gridWidth, gridHeight, tileSize, gridData);
}

void LoadLevel::LoadLayers(const json& levelJson) {
    if (!levelJson.contains("layers") || !levelJson["layers"].is_array()) {
        return;
    }

    for (const auto& layer : levelJson["layers"]) {
        LoadLayer(layer);
    }
}

void LoadLevel::LoadLayer(const json& layerJson) {
    const std::string name = layerJson.value("name", "");

    if (name == "Spikes") {
        LoadSpikes(layerJson);
    } else if (name == "Moving_enemy") {
        LoadMovingEnemies(layerJson);
    } else if (name == "DisappearingPlatformGroup") {
        LoadDisappearingPlatformGroups(layerJson);
    } else if (name == "MovingPlatform") {
        LoadMovingPlatforms(layerJson);
    } else if (name == "CheckPoint") {
        LoadCheckPoints(layerJson);
    }
}

void LoadLevel::LoadSpikes(const json& layerJson) {
    const std::string imagePath = layerJson.value("image_path", "");
    if (imagePath.empty() || !layerJson.contains("positions") || !layerJson["positions"].is_array()) {
        return;
    }

    for (const auto& pos : layerJson["positions"]) {
        const glm::vec2 screenPos = GridPositionToScreen(pos);

        auto spike = std::make_shared<Spike>(screenPos, imagePath);
        spike->SetZIndex(5.0f);
        m_Hazards.push_back(spike);
    }
}

void LoadLevel::LoadMovingEnemies(const json& layerJson) {
    const std::vector<std::string> animPaths = GetImagePaths(layerJson);
    if (animPaths.empty() || !layerJson.contains("moving_paths") || !layerJson["moving_paths"].is_array()) {
        return;
    }

    const float scaleValue = layerJson.value("scale", 2.0f);
    const float speedValue = layerJson.value("speed", 100.0f);

    for (const auto& path : layerJson["moving_paths"]) {
        if (!path.contains("start") || !path.contains("end")) {
            continue;
        }

        const glm::vec2 startPos = GridPositionToScreen(path["start"]);
        const glm::vec2 endPos = GridPositionToScreen(path["end"]);

        auto enemy = std::make_shared<MovingEnemy>(startPos, endPos, animPaths, scaleValue, speedValue);
        enemy->SetZIndex(10.0f);
        m_Hazards.push_back(enemy);
    }
}

void LoadLevel::LoadDisappearingPlatformGroups(const json& layerJson) {
    const std::vector<std::string> imagePaths = GetImagePaths(layerJson);
    if (imagePaths.empty() || !layerJson.contains("positions") || !layerJson["positions"].is_array()) {
        return;
    }

    const auto& positionsArray = layerJson["positions"];
    if (!positionsArray.empty() && positionsArray[0].is_array()) {
        for (const auto& groupPositions : positionsArray) {
            if (!groupPositions.is_array()) {
                continue;
            }

            std::vector<glm::vec2> positions;
            for (const auto& pos : groupPositions) {
                if (!pos.is_object()) {
                    continue;
                }
                positions.push_back(GridPositionToScreen(pos));
            }

            if (!positions.empty()) {
                auto group = std::make_shared<DisappearingPlatformGroup>(positions, imagePaths, 3.0f);
                m_Platforms.push_back(group);
            }
        }
        return;
    }

    std::vector<glm::vec2> positions;
    for (const auto& pos : positionsArray) {
        if (!pos.is_object()) {
            continue;
        }
        positions.push_back(GridPositionToScreen(pos));
    }

    if (!positions.empty()) {
        auto group = std::make_shared<DisappearingPlatformGroup>(positions, imagePaths, 3.0f);
        m_Platforms.push_back(group);
    }
}

void LoadLevel::LoadMovingPlatforms(const json& layerJson) {
    const std::vector<std::string> imagePaths = GetImagePaths(layerJson);
    if (imagePaths.empty() || !layerJson.contains("positions") || !layerJson["positions"].is_array()) {
        return;
    }

    const float scaleValue = layerJson.value("scale", 3.0f);
    const float speedValue = layerJson.value("speed", 200.0f);
    const auto& positionsArray = layerJson["positions"];
    const size_t numPlatforms = std::min(imagePaths.size(), positionsArray.size());

    for (size_t i = 0; i < numPlatforms; ++i) {
        const auto& pos = positionsArray[i];
        if (!pos.is_object() || !pos.contains("original") || !pos.contains("start") || !pos.contains("end")) {
            continue;
        }

        const auto& original = pos["original"];
        const auto& start = pos["start"];
        const auto& end = pos["end"];
        if (!original.is_object() || !start.is_object() || !end.is_object()) {
            continue;
        }

        const glm::vec2 originalPos = GridPositionToScreen(original);
        const glm::vec2 startPos = GridPositionToScreen(start);
        const glm::vec2 endPos = GridPositionToScreen(end);

        auto platform = std::make_shared<MovingPlatform>(originalPos, startPos, endPos, imagePaths[i], scaleValue, speedValue);
        m_Platforms.push_back(platform);
    }
}

void LoadLevel::LoadCheckPoints(const json& layerJson) {
    std::string imagePath;
    const std::string type = layerJson.value("type", "");
    if (type == "up") {
        imagePath = "Resources/Trigger/trigger_5.png";
    } else if (type == "down") {
        imagePath = "Resources/Trigger/trigger_4.png";
    }

    if (imagePath.empty() || !layerJson.contains("positions") || !layerJson["positions"].is_array()) {
        return;
    }

    for (const auto& pos : layerJson["positions"]) {
        const glm::vec2 screenPos = GridPositionToScreen(pos);

        auto checkPoint = std::make_shared<CheckPoint>(screenPos, imagePath);
        checkPoint->SetZIndex(5.0f);
        m_Triggers.push_back(checkPoint);
    }
}

glm::vec2 LoadLevel::GridPositionToScreen(const json& positionJson) const {
    const float col = positionJson.value("col", 0.0f);
    const float row = positionJson.value("row", 0.0f);
    return m_TileMap->GridToScreen(col, row);
}

void LoadLevel::Draw() {
    m_Background.Draw();
    for (auto& hazard : m_Hazards) {
        hazard->Draw();
    }
    for (auto& trigger : m_Triggers) {
        trigger->Draw();
    }
    for (auto& platform : m_Platforms) {
        platform->Draw();
    }
}
