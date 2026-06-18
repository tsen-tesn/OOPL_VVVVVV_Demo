#include "GameScene.hpp"

#include "Entity/Trigger/CheckPoint.hpp"
#include "AudioManager.hpp"
#include "Util/Color.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

namespace {
const std::string kFont = "Resources/Font/space-station.ttf";
constexpr int kFinalRoomID = 25;
constexpr float kCompleteJingleDuration = 3.1f;
constexpr float kCompleteTransitionDuration = 0.8f;
constexpr glm::vec2 kCompleteBannerCenter = {0.0f, 0.0f};
constexpr float kCompleteBannerScale = 2.0f;
constexpr glm::vec2 kCheatModeLabelPosition = {0.0f, -300.0f};
constexpr float kCheatModeLabelDuration = 1.5f;
}

// 建構子 / 初始化
GameScene::GameScene() {
    m_LevelManager = std::make_unique<LevelManager>(1);
    m_Player = std::make_shared<Player>(m_LevelManager->GetCurrentLevel()->GetTileMap());
    m_Player->SetRespawnPos({-200.0f, -150.0f});
    m_GameCompleteBanner = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>("Resources/Font/gamecomplete.png"),
        100.0f
    );
    m_GameCompleteBanner->m_Transform.translation = kCompleteBannerCenter;
    m_GameCompleteBanner->m_Transform.scale = {kCompleteBannerScale, kCompleteBannerScale};

    m_CheatModeOnLabelObject = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            kFont,
            20,
            "Cheat mode ON",
            Util::Color::FromRGB(120, 220, 120)
        ),
        100.0f
    );
    m_CheatModeOnLabelObject->m_Transform.translation = kCheatModeLabelPosition;

    m_CheatModeOffLabelObject = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            kFont,
            20,
            "Cheat mode OFF",
            Util::Color::FromRGB(150, 150, 150)
        ),
        100.0f
    );
    m_CheatModeOffLabelObject->m_Transform.translation = kCheatModeLabelPosition;

    RefreshCurrentLevelBindings();

    AudioManager::GetInstance().PlayGameBgm();
}

// 主更新迴圈
void GameScene::Update() {
    if (m_IsGameComplete) {
        const float dt = std::min(Util::Time::GetDeltaTimeMs() / 1000.0f, 0.05f);
        m_GameCompleteTimer += dt;

        const float transitionStart = kCompleteJingleDuration;
        const float transitionEnd = transitionStart + kCompleteTransitionDuration;
        if (m_GameCompleteTimer >= transitionEnd) {
            m_NextScene = SceneType::Menu;
            return;
        }

        if (m_GameCompleteTimer >= transitionStart) {
            const float progress = (m_GameCompleteTimer - transitionStart) / kCompleteTransitionDuration;
            const float scale = kCompleteBannerScale * (1.0f - 0.7f * progress);
            m_GameCompleteBanner->m_Transform.translation = {
                kCompleteBannerCenter.x,
                kCompleteBannerCenter.y + 160.0f * progress
            };
            m_GameCompleteBanner->m_Transform.scale = {scale, scale};
        }

        if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
            Util::Input::IfExit()) {
            m_ShouldQuit = true;
        }
        return;
    }

    HandleCheatInput();
    if (m_CheatModeLabelTimer > 0.0f) {
        const float dt = std::min(Util::Time::GetDeltaTimeMs() / 1000.0f, 0.05f);
        m_CheatModeLabelTimer = std::max(0.0f, m_CheatModeLabelTimer - dt);
    }

    const bool wasDead = m_Player->IsDead();

    UpdatePlatforms();
    m_Player->Update();

    HandleRespawn(wasDead);
    HandleCheckPoints();
    HandleHazards();
    HandleRoomTransition();

    // 離開
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_ShouldQuit = true;
    }
}

void GameScene::Draw() {
    m_LevelManager->GetCurrentLevel()->Draw();
    if (m_IsGameComplete) {
        m_GameCompleteBanner->Draw();
    } else {
        m_Player->Draw();
        if (m_CheatModeLabelTimer > 0.0f) {
            if (m_IsInvincibleCheatEnabled) {
                m_CheatModeOnLabelObject->Draw();
            } else {
                m_CheatModeOffLabelObject->Draw();
            }
        }
    }
}

// 子步驟
void GameScene::HandleCheatInput() {
    if (Util::Input::IsKeyDown(Util::Keycode::C)) {
        m_IsInvincibleCheatEnabled = !m_IsInvincibleCheatEnabled;
        m_CheatModeLabelTimer = kCheatModeLabelDuration;
        LOG_INFO("Invincible cheat {}", m_IsInvincibleCheatEnabled ? "enabled" : "disabled");
    }
}

void GameScene::RefreshCurrentLevelBindings() {
    const auto& level = m_LevelManager->GetCurrentLevel();
    m_Platforms = level->GetPlatforms();
    m_Player->SetTileMap(level->GetTileMap());
    m_Player->SetPlatforms(m_Platforms);
}

void GameScene::UpdatePlatforms() {
    for (const auto& platform : m_Platforms) {
        if (auto group = std::dynamic_pointer_cast<DisappearingPlatformGroup>(platform)) {
            group->CheckCollisionAndDisappear(m_Player->GetPosition());
        }
        platform->Update();
    }
}

void GameScene::HandleRespawn(bool wasDead) {
    // 剛從死亡恢復：若復活點在另一個房間則切換
    if (wasDead && !m_Player->IsDead()) {
        if (m_LevelManager->GetCurrentRoomID() != m_LevelManager->GetRespawnRoomID()) {
            m_LevelManager->LoadRoom(m_LevelManager->GetRespawnRoomID());
            RefreshCurrentLevelBindings();
        }

        for (const auto& platform : m_Platforms) {
            platform->Reset();
        }
    }
}

void GameScene::HandleCheckPoints() {
    const auto& level = m_LevelManager->GetCurrentLevel();

    for (const auto& trigger : level->GetTriggers()) {
        const auto checkpoint = std::dynamic_pointer_cast<CheckPoint>(trigger);
        if (!checkpoint) {
            continue;
        }

        if (checkpoint->IsTouched(m_Player->GetPosition())) {
            if (m_LevelManager->GetCurrentRoomID() == kFinalRoomID) {
                TriggerGameComplete();
                return;
            }

            if (!checkpoint->IsActivated()) {
                // 取消所有存檔點，啟用當前
                for (const auto& otherTrigger : level->GetTriggers()) {
                    const auto cp = std::dynamic_pointer_cast<CheckPoint>(otherTrigger);
                    if (!cp) {
                        continue;
                    }
                    cp->SetActivated(false);
                }
                checkpoint->SetActivated(true);

                glm::vec2 safePos;
                safePos.x = checkpoint->GetTransform().translation.x;
                safePos.y = m_Player->GetPosition().y;
                m_Player->SetRespawnPos(safePos);
                m_LevelManager->SetRespawnRoomID(m_LevelManager->GetCurrentRoomID());
                AudioManager::GetInstance().PlaySave();
                LOG_INFO("Checkpoint Saved");
            }
        }
    }
}

void GameScene::HandleHazards() {
    for (const auto& hazard : m_LevelManager->GetCurrentLevel()->GetHazards()) {
        hazard->Update();
        if (hazard->is_touched(m_Player->GetPosition())) {
            if (m_IsInvincibleCheatEnabled) {
                continue;
            }
            m_Player->Die();
        }
    }
}

// 房間切換
void GameScene::HandleRoomTransition() {
    glm::vec2 pos = m_Player->GetTransform().translation;
    const bool changedRoom = m_LevelManager->TryTransition(pos);

    if (pos != m_Player->GetTransform().translation) {
        m_Player->m_Transform.translation = pos;
    }

    if (changedRoom) {
        RefreshCurrentLevelBindings();
    }
}

void GameScene::TriggerGameComplete() {
    if (m_IsGameComplete) {
        return;
    }

    m_IsGameComplete = true;
    m_GameCompleteTimer = 0.0f;
    m_GameCompleteBanner->m_Transform.translation = kCompleteBannerCenter;
    m_GameCompleteBanner->m_Transform.scale = {kCompleteBannerScale, kCompleteBannerScale};
    AudioManager::GetInstance().PauseGameBgm();
    AudioManager::GetInstance().PlayGameComplete();
}
