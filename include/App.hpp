#ifndef APP_HPP
#define APP_HPP

#include <vector>

#include "pch.hpp" // IWYU pragma: export
#include "Player.hpp"
#include "Hazard.hpp"
#include "Spike.hpp"
#include "Loadlevel.hpp"
#include "MovingEnemy.hpp"  
#include "TileMap.hpp"
#include "MovingPlatform.hpp"
#include "DisappearingPlatformGroup.hpp"

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
    std::vector<std::shared_ptr<DisappearingPlatformGroup>> m_DisappearingPlatformGroups;
};

#endif
