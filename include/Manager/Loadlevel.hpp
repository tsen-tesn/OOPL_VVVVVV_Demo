#pragma once
#ifndef LOADLEVEL_HPP
#define LOADLEVEL_HPP

#include <string>
#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Entity/TileMap.hpp"
#include "Entity/Hazard/Spike.hpp"
#include "Entity/Platform/Platform.hpp"
#include "Entity/Trigger/CheckPoint.hpp"

class LoadLevel {
public:
    struct Connections {
        int up = -1;
        int right = -1;
        int down = -1;
        int left = -1;
    };

    explicit LoadLevel(const std::string& jsonPath);

    void Draw();
    
    Connections GetConnections() const { return m_Connections; }
    std::shared_ptr<TileMap> GetTileMap() const { return m_TileMap; }
    const std::vector<std::shared_ptr<Hazard>>& GetHazards() const { return m_Hazards; }
    const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const { return m_Platforms; }
    const std::vector<std::shared_ptr<CheckPoint>>& GetCheckPoints() const { return m_Trigger; }

private:
    Connections m_Connections;
    std::shared_ptr<TileMap> m_TileMap;
    Util::GameObject m_Background;
    std::vector<std::shared_ptr<Hazard>> m_Hazards;
    std::vector<std::shared_ptr<Platform>> m_Platforms;
    std::vector<std::shared_ptr<CheckPoint>> m_Trigger;
    CheckPoint* m_CurrentCheckPoint = nullptr;
};

#endif