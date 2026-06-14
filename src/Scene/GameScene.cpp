#include "GameScene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// 建構子 / 初始化
// ─────────────────────────────────────────────────────────────────────────────
GameScene::GameScene() {
    LoadRoom(m_CurrentRoomID, {-200.0f, -150.0f});
}

void GameScene::LoadRoom(int roomID, const glm::vec2& spawnPos) {
    const std::string path =
        std::string(RESOURCE_DIR) + "/Map/VVVVVV Demo/room" + std::to_string(roomID) + ".json";

    m_Level     = std::make_shared<LoadLevel>(path);
    m_Platforms = m_Level->GetPlatforms();

    if (!m_Player) {
        m_Player = std::make_shared<Player>(m_Level->GetTileMap());
        m_Player->SetRespawnPos(spawnPos);
    } else {
        m_Player->SetTileMap(m_Level->GetTileMap());
    }

    m_Player->SetPlatforms(m_Platforms);
    m_CurrentRoomID = roomID;
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

    // 繪製（背景在前）
    m_Level->Draw();
    m_Player->Update();
    m_Player->Draw();

    UpdatePlatforms();
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

// ─────────────────────────────────────────────────────────────────────────────
// 子步驟
// ─────────────────────────────────────────────────────────────────────────────
void GameScene::UpdatePlatforms() {
    for (const auto& platform : m_Platforms) {
        if (auto group = std::dynamic_pointer_cast<DisappearingPlatformGroup>(platform)) {
            group->CheckCollisionAndDisappear(m_Player);
        }
        platform->Update();
    }
}

void GameScene::HandleRespawn(bool wasDead) {
    // 剛從死亡恢復：若復活點在另一個房間則切換
    if (wasDead && !m_Player->IsDead()) {
        if (m_CurrentRoomID != m_RespawnRoomID) {
            LoadRoom(m_RespawnRoomID, m_Player->GetPosition());
            m_Player->SetTileMap(m_Level->GetTileMap());
        }
    }
}

void GameScene::HandleCheckPoints() {
    for (const auto& checkpoint : m_Level->GetCheckPoints()) {
        if (checkpoint->IsTouched(m_Player->GetPosition())) {
            if (!checkpoint->IsActivated()) {
                // 取消所有存檔點，啟用當前
                for (const auto& cp : m_Level->GetCheckPoints())
                    cp->SetActivated(false);
                checkpoint->SetActivated(true);

                glm::vec2 safePos;
                safePos.x = checkpoint->GetTransform().translation.x;
                safePos.y = m_Player->GetPosition().y;
                m_Player->SetRespawnPos(safePos);
                m_RespawnRoomID = m_CurrentRoomID;
                LOG_INFO("Checkpoint Saved");
            }
        }
    }
}

void GameScene::HandleHazards() {
    for (const auto& hazard : m_Level->GetHazards()) {
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
    glm::vec2 pos  = m_Player->GetTransform().translation;
    auto      conn = m_Level->GetConnections();

    struct Edge {
        float    playerCoord;   // pos.x 或 pos.y
        float    halfExtent;    // kHalfW 或 kHalfH
        bool     exceeded;      // 是否超過邊界
        int      connID;        // 對應連線 roomID
        float    wrapCoord;     // 進入新房間後的座標
        bool     isX;           // true = X 軸邊界
    };

    const Edge edges[] = {
        // Right
        {pos.x,  kHalfW, pos.x >=  kHalfW, conn.right,  -kHalfW + 10.0f, true },
        // Left
        {pos.x,  kHalfW, pos.x <= -kHalfW, conn.left,    kHalfW - 10.0f, true },
        // Top
        {pos.y,  kHalfH, pos.y >=  kHalfH, conn.up,     -kHalfH + 10.0f, false},
        // Bottom
        {pos.y,  kHalfH, pos.y <= -kHalfH, conn.down,    kHalfH - 10.0f, false},
    };

    for (const auto& edge : edges) {
        if (!edge.exceeded) continue;

        if (edge.connID == -1) {
            // 阻擋
            if (edge.isX) pos.x = std::copysign(kHalfW, pos.x);
            else          pos.y = std::copysign(kHalfH, pos.y);
        } else {
            try {
                LoadRoom(edge.connID, m_Player->GetPosition());
                if (edge.isX) pos.x = edge.wrapCoord;
                else          pos.y = edge.wrapCoord;
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to load room {}: {}", edge.connID, e.what());
                if (edge.isX) pos.x = std::copysign(kHalfW, pos.x);
                else          pos.y = std::copysign(kHalfH, pos.y);
            }
        }
        break; // 一次只處理一個邊界
    }

    if (pos != m_Player->GetTransform().translation) {
        m_Player->m_Transform.translation = pos;
    }
}
