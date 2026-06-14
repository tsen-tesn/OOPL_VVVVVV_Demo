#include "Player.hpp"
#include "DisappearingPlatformGroup.hpp"
#include "Util/Logger.hpp"

#include <memory>
#include <string>
#include <vector>

// ─────────────────────────────────────────────
// 動畫索引對應（用 2-bit 組合）
//   facing:  0 = right, 1 = left
//   gravity: 0 = down,  1 = up
//
//   idx = facing * 2 + gravity
//   0: right+down  1: right+up
//   2: left+down   3: left+up
// ─────────────────────────────────────────────
namespace {
    constexpr int AnimIdx(bool facingLeft, bool gravityUp) {
        return (facingLeft ? 2 : 0) + (gravityUp ? 1 : 0);
    }

    std::shared_ptr<Util::Animation> CreateAnimation(int start, int end) {
        std::vector<std::string> frames;
        for (int i = start; i <= end; ++i)
            frames.push_back("Resources/Character/Main/role_" + std::to_string(i) + ".png");
        return std::make_shared<Util::Animation>(frames, true, 100, true);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 建構子
// ─────────────────────────────────────────────────────────────────────────────
Player::Player(std::shared_ptr<TileMap> tileMap)
    : m_TileMap(tileMap) {
    // 走路動畫： 0~2 right+down, 3~5 left+down(=right+up frame), 6~8 right+up, 9~11 left+up
    // 原始檔案對應：
    //   right+down = role_6~8,  left+down = role_9~11
    //   right+up   = role_0~2,  left+up   = role_3~5
    m_Animations[AnimIdx(false, false)] = CreateAnimation(6,  8);   // right+down
    m_Animations[AnimIdx(false, true )] = CreateAnimation(0,  2);   // right+up
    m_Animations[AnimIdx(true,  false)] = CreateAnimation(9,  11);  // left+down
    m_Animations[AnimIdx(true,  true )] = CreateAnimation(3,  5);   // left+up

    // Idle 靜止圖（各動畫第一幀）
    m_IdleImages[AnimIdx(false, false)] = std::make_shared<Util::Image>("Resources/Character/Main/role_6.png");
    m_IdleImages[AnimIdx(false, true )] = std::make_shared<Util::Image>("Resources/Character/Main/role_0.png");
    m_IdleImages[AnimIdx(true,  false)] = std::make_shared<Util::Image>("Resources/Character/Main/role_9.png");
    m_IdleImages[AnimIdx(true,  true )] = std::make_shared<Util::Image>("Resources/Character/Main/role_3.png");

    // 死亡閃爍圖
    m_DeathImages[AnimIdx(false, false)] = std::make_shared<Util::Image>("Resources/Character/Main/role_22.png");
    m_DeathImages[AnimIdx(false, true )] = std::make_shared<Util::Image>("Resources/Character/Main/role_16.png");
    m_DeathImages[AnimIdx(true,  false)] = std::make_shared<Util::Image>("Resources/Character/Main/role_25.png");
    m_DeathImages[AnimIdx(true,  true )] = std::make_shared<Util::Image>("Resources/Character/Main/role_19.png");

    m_Drawable         = m_Animations[AnimIdx(false, false)];
    m_ZIndex           = 50.0f;
    m_Transform.scale  = {3.0f, 3.0f};
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 主迴圈（骨架）
// ─────────────────────────────────────────────────────────────────────────────
void Player::Update() {
    const float dt = std::min(Util::Time::GetDeltaTimeMs() / 1000.0f, 0.05f);

    if (m_IsDead) {
        HandleDeathAnimation(dt);
        return;
    }

    HandleInput();
    ApplyPhysics(dt);
    ApplyPlatformRiding();
    UpdateAnimation();
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 子步驟
// ─────────────────────────────────────────────────────────────────────────────
void Player::HandleDeathAnimation(float dt) {
    m_DeathTimer += dt;

    // 閃爍：偶數 phase 顯示死亡圖，奇數 phase 隱藏
    const int blinkPhase = static_cast<int>(m_DeathTimer / m_BlinkInterval);
    if (blinkPhase % 2 == 0)
        m_Drawable = SelectDeathImage();
    else
        m_Drawable = nullptr;

    if (m_DeathTimer >= m_DeathDuration) {
        Respawn();
        m_Drawable = SelectAnimation(false); // idle
    }
}

void Player::HandleInput() {
    constexpr float kMoveSpeed = 400.0f;

    m_Velocity.x = 0.0f;

    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        m_Velocity.x = -kMoveSpeed;
        m_FacingRight = false;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        m_Velocity.x = kMoveSpeed;
        m_FacingRight = true;
    }

    // 只有貼地時才允許翻轉重力
    if (IsOnSurface()) {
        if (Util::Input::IsKeyDown(Util::Keycode::DOWN))
            m_GravityDown = false;
        else if (Util::Input::IsKeyDown(Util::Keycode::UP))
            m_GravityDown = true;
        else if (Util::Input::IsKeyDown(Util::Keycode::SPACE))
            m_GravityDown = !m_GravityDown;
    }
}

void Player::ApplyPhysics(float dt) {
    const float gravDir = m_GravityDown ? 1.0f : -1.0f;
    m_Velocity.y = m_Gravity * gravDir;

    // X 軸
    glm::vec2 nextX = m_Transform.translation;
    nextX.x += m_Velocity.x * dt;
    if (CanMoveTo(nextX))
        m_Transform.translation.x = nextX.x;
    else
        m_Velocity.x = 0.0f;

    // Y 軸
    glm::vec2 nextY = m_Transform.translation;
    nextY.y += m_Velocity.y * dt;
    if (CanMoveTo(nextY))
        m_Transform.translation.y = nextY.y;
    else
        m_Velocity.y = 0.0f;
}

void Player::ApplyPlatformRiding() {
    const glm::vec2 delta = GetRidingPlatformDelta();
    if (delta.x != 0.0f || delta.y != 0.0f)
        m_Transform.translation += delta;
}

void Player::UpdateAnimation() {
    const bool isMoving = (m_Velocity.x != 0.0f);
    m_Drawable = SelectAnimation(isMoving);
}

// ─────────────────────────────────────────────────────────────────────────────
// 動畫輔助
// ─────────────────────────────────────────────────────────────────────────────
std::shared_ptr<Core::Drawable> Player::SelectAnimation(bool isMoving) const {
    const int idx = AnimIdx(!m_FacingRight, !m_GravityDown);
    if (isMoving)
        return m_Animations[idx];
    return m_IdleImages[idx];
}

std::shared_ptr<Core::Drawable> Player::SelectDeathImage() const {
    return m_DeathImages[AnimIdx(!m_FacingRight, !m_GravityDown)];
}

// ─────────────────────────────────────────────────────────────────────────────
// 外部介面
// ─────────────────────────────────────────────────────────────────────────────
void Player::SetTileMap(std::shared_ptr<TileMap> tileMap) {
    m_TileMap = tileMap;
}

glm::vec2 Player::GetPosition() const {
    return m_Transform.translation;
}

void Player::SetRespawnPos(const glm::vec2& pos) {
    m_RespawnPos          = pos;
    m_RespawnGravityDown  = m_GravityDown;
}

void Player::Respawn() {
    const float offset = m_RespawnGravityDown ? -9.0f : 9.0f;
    m_Transform.translation = m_RespawnPos + glm::vec2(0.0f, offset);
    m_Velocity              = {0.0f, 0.0f};
    m_GravityDown           = m_RespawnGravityDown;
    m_IsDead                = false;
    m_DeathTimer            = 0.0f;
}

void Player::Die() {
    if (m_IsDead) return;
    LOG_INFO("Player died");
    m_IsDead     = true;
    m_DeathTimer = 0.0f;
    m_Velocity   = {0.0f, 0.0f};
}

bool Player::IsDead() const {
    return m_IsDead;
}

void Player::SetPlatforms(const std::vector<std::shared_ptr<Platform>>& platforms) {
    m_Platforms.clear();
    for (const auto& p : platforms) {
        if (auto group = std::dynamic_pointer_cast<DisappearingPlatformGroup>(p)) {
            for (const auto& sub : group->GetPlatforms())
                m_Platforms.push_back(sub);
        } else {
            m_Platforms.push_back(p);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 碰撞查詢（共用 kHalfW / kHalfH / kShrink* 常數）
// ─────────────────────────────────────────────────────────────────────────────
glm::vec2 Player::GetRidingPlatformDelta() const {
    const float playerLeft  = m_Transform.translation.x - kHalfW + kShrinkLeft;
    const float playerRight = m_Transform.translation.x + kHalfW - kShrinkRight;

    for (const auto& platform : m_Platforms) {
        if (!platform || !platform->IsSolid()) continue;
        const glm::vec2 pPos  = platform->GetPosition();
        const glm::vec2 pHalf = platform->GetHalfSize();
        const float platL = pPos.x - pHalf.x;
        const float platR = pPos.x + pHalf.x;
        const float platT = pPos.y - pHalf.y;
        const float platB = pPos.y + pHalf.y;

        if (playerRight <= platL || playerLeft >= platR) continue;

        if (m_GravityDown) {
            const float feetY = m_Transform.translation.y + kHalfH - kShrinkBot + kProbeOffset;
            if (feetY >= platT && feetY <= platB) return platform->GetDelta();
        } else {
            const float headY = m_Transform.translation.y - kHalfH + kShrinkTop - kProbeOffset;
            if (headY <= platB && headY >= platT) return platform->GetDelta();
        }
    }
    return {0.0f, 0.0f};
}

bool Player::CanMoveTo(const glm::vec2& position) const {
    if (!m_TileMap) return true;

    // 四個角（已 shrink）
    const glm::vec2 corners[4] = {
        {position.x - kHalfW + kShrinkLeft,  position.y - kHalfH + kShrinkTop},
        {position.x + kHalfW - kShrinkRight, position.y - kHalfH + kShrinkTop},
        {position.x - kHalfW + kShrinkLeft,  position.y + kHalfH - kShrinkBot},
        {position.x + kHalfW - kShrinkRight, position.y + kHalfH - kShrinkBot},
    };

    for (const auto& c : corners) {
        const glm::ivec2 g = m_TileMap->ScreenToGrid(c);
        // 左右超出地圖：放行，交給房間切換邏輯處理
        if (g.x < 0 || g.x >= m_TileMap->GetGridWidth()) continue;
        // 上下超出地圖：視為可通過（上下無限）
        if (g.y < 0 || g.y >= m_TileMap->GetGridHeight()) continue;
        if (m_TileMap->GetTileType(g.x, g.y) != TileMap::TileType::Path) return false;
    }

    // Platform AABB
    const float pLeft   = position.x - kHalfW + kShrinkLeft;
    const float pRight  = position.x + kHalfW - kShrinkRight;
    const float pTop    = position.y - kHalfH + kShrinkTop;
    const float pBottom = position.y + kHalfH - kShrinkBot;

    for (const auto& platform : m_Platforms) {
        if (!platform || !platform->IsSolid()) continue;
        const glm::vec2 pPos  = platform->GetPosition();
        const glm::vec2 pHalf = platform->GetHalfSize();
        if (pRight  > pPos.x - pHalf.x &&
            pLeft   < pPos.x + pHalf.x &&
            pBottom > pPos.y - pHalf.y &&
            pTop    < pPos.y + pHalf.y) {
            return false;
        }
    }
    return true;
}

bool Player::IsOnSurface() const {
    if (!m_TileMap) return false;

    const float playerLeft  = m_Transform.translation.x - kHalfW + kShrinkLeft;
    const float playerRight = m_Transform.translation.x + kHalfW - kShrinkRight;

    glm::vec2 lp, rp;
    if (m_GravityDown) {
        const float feetY = m_Transform.translation.y + kHalfH - kShrinkBot + kProbeOffset;
        lp = {playerLeft,  feetY};
        rp = {playerRight, feetY};
    } else {
        const float headY = m_Transform.translation.y - kHalfH + kShrinkTop - kProbeOffset;
        lp = {playerLeft,  headY};
        rp = {playerRight, headY};
    }

    auto isWall = [this](const glm::vec2& p) {
        const glm::ivec2 g = m_TileMap->ScreenToGrid(p);
        if (g.x < 0 || g.x >= m_TileMap->GetGridWidth())  return false;
        if (g.y < 0 || g.y >= m_TileMap->GetGridHeight()) return false;
        return m_TileMap->GetTileType(g.x, g.y) == TileMap::TileType::Wall;
    };

    if (isWall(lp) || isWall(rp)) return true;

    // Platform surface probe
    for (const auto& platform : m_Platforms) {
        if (!platform || !platform->IsSolid()) continue;
        const glm::vec2 pPos  = platform->GetPosition();
        const glm::vec2 pHalf = platform->GetHalfSize();
        if (playerRight <= pPos.x - pHalf.x || playerLeft >= pPos.x + pHalf.x) continue;

        if (m_GravityDown) {
            const float feetY = m_Transform.translation.y + kHalfH - kShrinkBot + kProbeOffset;
            if (feetY >= pPos.y - pHalf.y && feetY <= pPos.y + pHalf.y) return true;
        } else {
            const float headY = m_Transform.translation.y - kHalfH + kShrinkTop - kProbeOffset;
            if (headY <= pPos.y + pHalf.y && headY >= pPos.y - pHalf.y) return true;
        }
    }
    return false;
}