#include "Player.hpp"
#include "Util/Logger.hpp"

#include <memory>
#include <string>
#include <vector>

namespace {
    std::shared_ptr<Util::Animation> CreateAnimation(int start, int end) {
        std::vector<std::string> frames;
        for (int i = start; i <= end; ++i) {
            frames.push_back("Resources/Character/Main/role_" + std::to_string(i) + ".png");
        }
        return std::make_shared<Util::Animation>(frames, true, 100, true);
    }
}

Player::Player(std::shared_ptr<TileMap> tileMap)
    : m_TileMap(tileMap) {
    // 0~2   : right + up
    // 3~5   : left  + up
    // 6~8   : right + down
    // 9~11  : left  + down

    m_RightUpAnimation = CreateAnimation(0, 2);
    m_LeftUpAnimation = CreateAnimation(3, 5);
    m_RightDownAnimation = CreateAnimation(6, 8);
    m_LeftDownAnimation = CreateAnimation(9, 11);

    m_RightDownImage = std::make_shared<Util::Image>("Resources/Character/Main/role_6.png");
    m_LeftDownImage = std::make_shared<Util::Image>("Resources/Character/Main/role_9.png");
    m_RightUpImage = std::make_shared<Util::Image>("Resources/Character/Main/role_0.png");
    m_LeftUpImage = std::make_shared<Util::Image>("Resources/Character/Main/role_3.png");

    m_RightDownDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_16.png");
    m_LeftDownDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_19.png");
    m_RightUpDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_22.png");
    m_LeftUpDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_25.png");

    // 預設狀態：面右、重力向下
    m_Drawable = m_RightDownAnimation;
    m_ZIndex = 50.0f;
    m_Transform.scale = {3.0f, 3.0f};
}

void Player::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (m_IsDead) {
        m_DeathTimer += deltaTime;

        int blinkPhase = static_cast<int>(m_DeathTimer / m_BlinkInterval);

        if (blinkPhase % 2 == 0) {
            if (m_FacingRight && m_GravityDown) {
                m_Drawable = m_RightUpDeathImage;
            }
            else if (!m_FacingRight && m_GravityDown) {
                m_Drawable = m_LeftUpDeathImage;
            }
            else if (m_FacingRight && !m_GravityDown) {
                m_Drawable = m_RightDownDeathImage;
            }
            else {
                m_Drawable = m_LeftDownDeathImage;
            }
        }
        else {
            m_Drawable = nullptr;
        }

        if (m_DeathTimer >= m_DeathDuration) {
            m_Transform.translation = {-200.0f, 0.0f};
            m_Velocity = {0.0f, 0.0f};
            m_IsDead = false;
            m_DeathTimer = 0.0f;

            if (m_FacingRight && m_GravityDown) {
                m_Drawable = m_RightDownAnimation;
            }
            else if (!m_FacingRight && m_GravityDown) {
                m_Drawable = m_LeftDownAnimation;
            }
            else if (m_FacingRight && !m_GravityDown) {
                m_Drawable = m_RightUpAnimation;
            }
            else {
                m_Drawable = m_LeftUpAnimation;
            }
        }

        return;
    }

    float moveSpeed = 400.0f;

    // 左右移動
    m_Velocity.x = 0.0f;

    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        m_Velocity.x = -moveSpeed;
        m_FacingRight = false;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        m_Velocity.x = moveSpeed;
        m_FacingRight = true;
    }

    // 只有貼地時才允許翻轉重力
    if (IsOnSurface()) {
        if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            m_GravityDown = false;
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
            m_GravityDown = true;
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
            m_GravityDown = !m_GravityDown;
        }
    }

    float gravityDirection = m_GravityDown ? 1.0f : -1.0f;
    m_Velocity.y = m_Gravity * gravityDirection;

    glm::vec2 nextPosX = m_Transform.translation;
    nextPosX.x += m_Velocity.x * deltaTime;
    
    if (CanMoveTo(nextPosX)) {
        m_Transform.translation.x = nextPosX.x;
    }
    else {
        m_Velocity.x = 0.0f;
    }

    glm::vec2 nextPosY = m_Transform.translation;
    nextPosY.y += m_Velocity.y * deltaTime;

    if (CanMoveTo(nextPosY)) {
        m_Transform.translation.y = nextPosY.y;
    }
    else {
        m_Velocity.y = 0.0f;
    }



    bool isMoving = (m_Velocity.x != 0.0f);

    if (m_FacingRight && m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_RightDownAnimation;
        }
        else {
            m_Drawable = m_RightDownImage;
        }
    }
    else if (!m_FacingRight && m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_LeftDownAnimation;
        }
        else {
            m_Drawable = m_LeftDownImage;
        }
    }
    else if (m_FacingRight && !m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_RightUpAnimation;
        }
        else {
            m_Drawable = m_RightUpImage;
        }
    }
    else {
        if (isMoving) {
            m_Drawable = m_LeftUpAnimation;
        }
        else {
            m_Drawable = m_LeftUpImage;
        }
    }

    m_Transform.scale = {3.0f, 3.0f};
}

void Player::SetTileMap(std::shared_ptr<TileMap> tileMap) {
    m_TileMap = tileMap;
}

glm::vec2 Player::GetPosition() const {
    return m_Transform.translation;
}


void Player::Die() {
    if (m_IsDead) {
        return;
    }

    LOG_INFO("Player died");
    m_IsDead = true;
    m_DeathTimer = 0.0f;
    m_Velocity = {0.0f, 0.0f};
}

bool Player::IsDead() const {
    return m_IsDead;
}

bool Player::CanMoveTo(const glm::vec2& position) const {
    if (!m_TileMap) {
        return true;
    }

    const float halfWidth  = 24.0f * 3.0f / 2.0f; // 36
    const float halfHeight = 24.0f * 3.0f / 2.0f; // 36

    const float shrinkLeft   = 21.0f;
    const float shrinkRight  = 20.0f;
    const float shrinkTop    = 5.0f;
    const float shrinkBottom = 9.0f;

    // === 四個角 ===
    glm::vec2 topLeft(
        position.x - halfWidth  + shrinkLeft,
        position.y - halfHeight + shrinkTop
    );

    glm::vec2 topRight(
        position.x + halfWidth  - shrinkRight,
        position.y - halfHeight + shrinkTop
    );

    glm::vec2 bottomLeft(
        position.x - halfWidth  + shrinkLeft,
        position.y + halfHeight - shrinkBottom
    );

    glm::vec2 bottomRight(
        position.x + halfWidth  - shrinkRight,
        position.y + halfHeight - shrinkBottom
    );

    auto canPassPoint = [this](const glm::vec2& p) -> bool {
        glm::ivec2 gridPos = m_TileMap->ScreenToGrid(p);

        // 左右超出地圖：先放行，交給 App 的換房邏輯處理
        if (gridPos.x < 0 || gridPos.x >= m_TileMap->GetGridWidth()) {
            return true;
        }

        // 上下超出地圖：仍然視為不可通過
        if (gridPos.y < 0 || gridPos.y >= m_TileMap->GetGridHeight()) {
            return true;
        }

        return m_TileMap->GetTileType(gridPos.x, gridPos.y) == TileMap::TileType::Path;
    };

    return canPassPoint(topLeft) &&
           canPassPoint(topRight) &&
           canPassPoint(bottomLeft) &&
           canPassPoint(bottomRight);
}

bool Player::IsOnSurface() const {
    if (!m_TileMap) {
        return false;
    }

    const float halfWidth  = 24.0f * 3.0f / 2.0f; // 36
    const float halfHeight = 24.0f * 3.0f / 2.0f; // 36

    const float shrinkLeft   = 21.0f;
    const float shrinkRight  = 20.0f;
    const float shrinkTop    = 5.0f;
    const float shrinkBottom = 9.0f;

    const float probeOffset = 10.0f; // 往外多探 1 pixel，避免剛好貼邊時誤判

    glm::vec2 leftPoint;
    glm::vec2 rightPoint;

    if (m_GravityDown) {
        // 檢查腳底左右兩點
        leftPoint = {
            m_Transform.translation.x - halfWidth + shrinkLeft,
            m_Transform.translation.y + halfHeight - shrinkBottom + probeOffset
        };

        rightPoint = {
            m_Transform.translation.x + halfWidth - shrinkRight,
            m_Transform.translation.y + halfHeight - shrinkBottom + probeOffset
        };
    } else {
        // 檢查頭頂左右兩點
        leftPoint = {
            m_Transform.translation.x - halfWidth + shrinkLeft,
            m_Transform.translation.y - halfHeight + shrinkTop - probeOffset
        };

        rightPoint = {
            m_Transform.translation.x + halfWidth - shrinkRight,
            m_Transform.translation.y - halfHeight + shrinkTop - probeOffset
        };
    }

    auto isWallPoint = [this](const glm::vec2& p) -> bool {
        glm::ivec2 gridPos = m_TileMap->ScreenToGrid(p);

        if (gridPos.x < 0 || gridPos.x >= m_TileMap->GetGridWidth()) {
            return false;
        }

        if (gridPos.y < 0 || gridPos.y >= m_TileMap->GetGridHeight()) {
            return false;
        }

        return m_TileMap->GetTileType(gridPos.x, gridPos.y) == TileMap::TileType::Wall;
    };

    return isWallPoint(leftPoint) || isWallPoint(rightPoint);
}

void Player::MoveX(float amount) {
    if (amount == 0.0f) {
        return;
    }

    float step = (amount > 0.0f) ? 1.0f : -1.0f;
    int moveCount = static_cast<int>(std::abs(amount));

    for (int i = 0; i < moveCount; ++i) {
        glm::vec2 nextPos = m_Transform.translation;
        nextPos.x += step;

        if (CanMoveTo(nextPos)) {
            m_Transform.translation.x = nextPos.x;
        }
        else {
            m_Velocity.x = 0.0f;
            return;
        }
    }

    float remainder = amount - step * moveCount;
    if (std::abs(remainder) > 0.0f) {
        glm::vec2 nextPos = m_Transform.translation;
        nextPos.x += remainder;

        if (CanMoveTo(nextPos)) {
            m_Transform.translation.x = nextPos.x;
        }
        else {
            m_Velocity.x = 0.0f;
        }
    }
}

void Player::MoveY(float amount) {
    if (amount == 0.0f) {
        return;
    }

    float step = (amount > 0.0f) ? 1.0f : -1.0f;
    int moveCount = static_cast<int>(std::abs(amount));

    for (int i = 0; i < moveCount; ++i) {
        glm::vec2 nextPos = m_Transform.translation;
        nextPos.y += step;

        if (CanMoveTo(nextPos)) {
            m_Transform.translation.y = nextPos.y;
        }
        else {
            m_Velocity.y = 0.0f;
            return;
        }
    }

    float remainder = amount - step * moveCount;
    if (std::abs(remainder) > 0.0f) {
        glm::vec2 nextPos = m_Transform.translation;
        nextPos.y += remainder;

        if (CanMoveTo(nextPos)) {
            m_Transform.translation.y = nextPos.y;
        }
        else {
            m_Velocity.y = 0.0f;
        }
    }
}