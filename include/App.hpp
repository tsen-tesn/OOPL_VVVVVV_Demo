#ifndef APP_HPP
#define APP_HPP

#include <vector>

#include "pch.hpp" // IWYU pragma: export
#include "Entity/Player.hpp"
#include "Entity/Hazard/Hazard.hpp"
#include "Entity/Hazard/Spike.hpp"
#include "Manager/Loadlevel.hpp"
#include "Entity/Hazard/MovingEnemy.hpp"  
#include "Entity/TileMap.hpp"
#include "Entity/Trigger/CheckPoint.hpp"
#include "Entity/Platform/MovingPlatform.hpp"
#include "Entity/Platform/DisappearingPlatformGroup.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::shared_ptr<LoadLevel> m_Level;
    std::vector<std::shared_ptr<Platform>> m_Platforms;
    int m_CurrentRoomID = 1;
    int m_RespawnRoomID = 1;
};

#endif
