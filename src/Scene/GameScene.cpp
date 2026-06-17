#include "GameScene.hpp"

#include "Entity/Trigger/CheckPoint.hpp"
#include "AudioManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// 建構子 / 初始化
// ─────────────────────────────────────────────────────────────────────────────
GameScene::GameScene() {
    m_LevelManager = std::make_unique<LevelManager>(1);
    m_Player = std::make_shared<Player>(m_LevelManager->GetCurrentLevel()->GetTileMap());
    m_Player->SetRespawnPos({-200.0f, -150.0f});
    RefreshCurrentLevelBindings();

    AudioManager::GetInstance().PlayGameBgm();
}

// ─────────────────────────────────────────────────────────────────────────────
// 主更新迴圈
// ─────────────────────────────────────────────────────────────────────────────
void GameScene::Update() {
    // 暫停輸入
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
        m_ShouldPause = true;
        return;
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
    m_Player->Draw();
}

// ─────────────────────────────────────────────────────────────────────────────
// 子步驟
// ─────────────────────────────────────────────────────────────────────────────
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
            m_Player->Die();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 房間切換（消除四方向重複程式碼）
// ─────────────────────────────────────────────────────────────────────────────
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
