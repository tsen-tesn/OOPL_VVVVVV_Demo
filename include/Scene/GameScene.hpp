#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Scene.hpp"
#include "Player.hpp"
#include "Loadlevel.hpp"
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
    // ─── 初始化 ──────────────────────────────────────────────────────
    void LoadRoom(int roomID, const glm::vec2& spawnPos);

    // ─── 每幀子步驟 ──────────────────────────────────────────────────
    void UpdatePlatforms();
    void HandleRespawn(bool wasDead);
    void HandleCheckPoints();
    void HandleHazards();
    void HandleRoomTransition();

private:
    std::shared_ptr<Player>   m_Player;
    std::shared_ptr<LoadLevel> m_Level;
    std::vector<std::shared_ptr<Platform>> m_Platforms;

    int m_CurrentRoomID = 1;
    int m_RespawnRoomID = 1;

    // 畫面半尺寸（房間邊界）
    static constexpr float kHalfW = 520.0f;
    static constexpr float kHalfH = 330.0f;

    bool m_ShouldQuit  = false;
    bool m_ShouldPause = false;
};

#endif // GAMESCENE_HPP
