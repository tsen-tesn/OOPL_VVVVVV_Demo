#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <memory>
#include <vector>

#include "Scene.hpp"
#include "Player.hpp"
#include "LevelManager.hpp"
#include "Platform.hpp"
#include "DisappearingPlatformGroup.hpp"

class GameScene : public Scene {
public:
    GameScene();

    void Update() override;
    void Draw() override;

    /// 回傳 true 代表玩家要求離開（ESC）
    bool ShouldQuit()  const { return m_ShouldQuit; }
    /// 回傳 true 代表玩家要求暫停
    bool ShouldPause() const { return m_ShouldPause; }

private:
    // ─── 每幀子步驟 ──────────────────────────────────────────────────
    void RefreshCurrentLevelBindings();
    void UpdatePlatforms();
    void HandleRespawn(bool wasDead);
    void HandleCheckPoints();
    void HandleHazards();
    void HandleRoomTransition();

private:
    std::shared_ptr<Player>   m_Player;
    std::unique_ptr<LevelManager> m_LevelManager;
    std::vector<std::shared_ptr<Platform>> m_Platforms;

    bool m_ShouldQuit  = false;
    bool m_ShouldPause = false;
};

#endif // GAMESCENE_HPP
