#pragma once
#ifndef LOADLEVEL_HPP
#define LOADLEVEL_HPP

#include <string>
#include <memory>
#include <vector>

#include "json.hpp"
#include "Util/GameObject.hpp"
#include "Entity/TileMap.hpp"
#include "Entity/Hazard/Spike.hpp"
#include "Entity/Platform/Platform.hpp"
#include "Entity/Trigger/Trigger.hpp"

class LoadLevel {
public:
    struct Connections {
        int up = -1;
        int right = -1;
        int down = -1;
        int left = -1;
    };

    explicit LoadLevel(const std::string& jsonPath);
    explicit LoadLevel(const nlohmann::json& levelJson, const std::string& entrySide = "");

    void Draw();
    
    Connections GetConnections() const { return m_Connections; }
    std::shared_ptr<TileMap> GetTileMap() const { return m_TileMap; }
    const std::vector<std::shared_ptr<Hazard>>& GetHazards() const { return m_Hazards; }
    const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const { return m_Platforms; }
    const std::vector<std::shared_ptr<Trigger>>& GetTriggers() const { return m_Triggers; }

private:
    void LoadConnections(const nlohmann::json& levelJson);
    void LoadBackground(const nlohmann::json& levelJson);
    void LoadTileMap(const nlohmann::json& levelJson);
    void LoadLayers(const nlohmann::json& levelJson);
    void LoadLayer(const nlohmann::json& layerJson);
    void LoadSpikes(const nlohmann::json& layerJson);
    void LoadMovingEnemies(const nlohmann::json& layerJson);
    void LoadDisappearingPlatformGroups(const nlohmann::json& layerJson);
    void LoadMovingPlatforms(const nlohmann::json& layerJson);
    void LoadCheckPoints(const nlohmann::json& layerJson);
    glm::vec2 GridPositionToScreen(const nlohmann::json& positionJson) const;

private:
    Connections m_Connections;
    std::string m_EntrySide;
    std::shared_ptr<TileMap> m_TileMap;
    Util::GameObject m_Background;
    std::vector<std::shared_ptr<Hazard>> m_Hazards;
    std::vector<std::shared_ptr<Platform>> m_Platforms;
    std::vector<std::shared_ptr<Trigger>> m_Triggers;
};

#endif
