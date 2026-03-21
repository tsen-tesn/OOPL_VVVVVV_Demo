#pragma once
#ifndef LOADLEVEL_HPP
#define LOADLEVEL_HPP

#include <string>
#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "TileMap.hpp"
#include "Spike.hpp"

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
    const std::vector<std::shared_ptr<Spike>>& GetSpikes() const { return m_Spikes; }

private:
    Connections m_Connections;
    std::shared_ptr<TileMap> m_TileMap;
    Util::GameObject m_Background;
    std::vector<std::shared_ptr<Spike>> m_Spikes;
};

#endif