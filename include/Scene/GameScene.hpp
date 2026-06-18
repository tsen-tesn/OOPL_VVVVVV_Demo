#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <memory>
#include <optional>
#include <vector>

#include "Scene.hpp"
#include "Player.hpp"
#include "LevelManager.hpp"
#include "Platform.hpp"
#include "DisappearingPlatformGroup.hpp"
#include "Util/GameObject.hpp"

namespace Util {
class Text;
}

class GameScene : public Scene {
public:
    GameScene();

    void Update() override;
    void Draw() override;

    /// 回傳 true 代表玩家要求離開（ESC）
    bool ShouldQuit()  const { return m_ShouldQuit; }
    std::optional<SceneType> GetNextScene() const override { return m_NextScene; }

private:
    // ─── 每幀子步驟 ──────────────────────────────────────────────────
    void HandleCheatInput();
    void RefreshCurrentLevelBindings();
    void UpdatePlatforms();
    void HandleRespawn(bool wasDead);
    void HandleCheckPoints();
    void HandleHazards();
    void HandleRoomTransition();
    void TriggerGameComplete();
    void UpdateCheatModeLabel();

private:
    std::shared_ptr<Player>   m_Player;
    std::unique_ptr<LevelManager> m_LevelManager;
    std::vector<std::shared_ptr<Platform>> m_Platforms;
    std::shared_ptr<Util::GameObject> m_GameCompleteBanner;
    std::shared_ptr<Util::GameObject> m_CheatModeLabelObject;
    std::shared_ptr<Util::Text> m_CheatModeLabel;

    bool m_ShouldQuit  = false;
    bool m_IsInvincibleCheatEnabled = false;
    bool m_IsGameComplete = false;
    float m_GameCompleteTimer = 0.0f;
    float m_CheatModeLabelTimer = 0.0f;
    std::optional<SceneType> m_NextScene;
};

#endif // GAMESCENE_HPP
