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

Player::Player() {
    // 0~3   : right + up
    // 4~7   : left  + up
    // 8~11  : right + down
    // 12~15 : left  + down

    m_RightUpAnimation = CreateAnimation(0, 2);
    m_LeftUpAnimation  = CreateAnimation(3, 5);
    m_RightDownAnimation   = CreateAnimation(6, 8); 
    m_LeftDownAnimation    = CreateAnimation(9, 11);

    m_RightDownImage = std::make_shared<Util::Image>("Resources/Character/Main/role_6.png");
    m_LeftDownImage = std::make_shared<Util::Image>("Resources/Character/Main/role_9.png");
    m_RightUpImage = std::make_shared<Util::Image>("Resources/Character/Main/role_0.png");
    m_LeftUpImage = std::make_shared<Util::Image>("Resources/Character/Main/role_3.png");
    m_RightDownDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_16.png");
    m_LeftDownDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_19.png");
    m_RightUpDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_22.png");
    m_LeftUpDeathImage = std::make_shared<Util::Image>("Resources/Character/Main/role_25.png");

    // 預設狀態：面右、往下
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
            // 偶數階段：顯示角色
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
            // 奇數階段：隱藏角色
            m_Drawable = nullptr;
        }

        if (m_DeathTimer >= m_DeathDuration) {
            m_Transform.translation = {-200.0f, 0.0f};
            m_Velocity = {0.0f, 0.0f};
            m_IsDead = false;
            m_DeathTimer = 0.0f;

            // 重生後恢復正常圖片
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

    float moveSpeed = 200.0f;

    // 左右移動：長按持續移動
    m_Velocity.x = 0.0f;

    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        m_Velocity.x = -moveSpeed;
        m_FacingRight = false;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        m_Velocity.x = moveSpeed;
        m_FacingRight = true;
    }


    if (IsGrounded()) {
        if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            m_GravityDown = false;
        } else if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
            m_GravityDown = true;
        } else if (Util::Input::IsKeyDown(Util::Keycode::SPACE)){
            m_GravityDown = !m_GravityDown;
        }
    }


    float gravityDirection = m_GravityDown ? 1.0f : -1.0f;
    m_Velocity.y = m_Gravity * gravityDirection;

    m_Transform.translation += m_Velocity * deltaTime;

    bool isMoving = (m_Velocity.x != 0.0f);

    if (m_FacingRight && m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_RightDownAnimation;
        } else {
            m_Drawable = m_RightDownImage;
        }
    }
    else if (!m_FacingRight && m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_LeftDownAnimation;
        } else {
            m_Drawable = m_LeftDownImage;
        }
    }
    else if (m_FacingRight && !m_GravityDown) {
        if (isMoving) {
            m_Drawable = m_RightUpAnimation;
        } else {
            m_Drawable = m_RightUpImage;
        }
    }
    else {
        if (isMoving) {
            m_Drawable = m_LeftUpAnimation;
        } else {
            m_Drawable = m_LeftUpImage;
        }
    }

    m_Transform.scale = {3.0f, 3.0f};

    // 畫面邊界 (對齊地圖 1080x720)
    const float leftBound   = -540.0f;
    const float rightBound  =  540.0f;
    const float topBound    = -330.0f;
    const float bottomBound =  330.0f;

    if (m_Transform.translation.x < leftBound) {
        m_Transform.translation.x = leftBound;
        m_Velocity.x = 0.0f;
    }
    else if (m_Transform.translation.x > rightBound) {
        m_Transform.translation.x = rightBound;
        m_Velocity.x = 0.0f;
    }

    if (m_Transform.translation.y < topBound) {
        m_Transform.translation.y = topBound;
        m_Velocity.y = 0.0f;
    }
    else if (m_Transform.translation.y > bottomBound) {
        m_Transform.translation.y = bottomBound;
        m_Velocity.y = 0.0f;
    }
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

bool Player::IsGrounded() {
    const float topBound = -300.0f;
    const float bottomBound = 300.0f;
    const float epsilon = 1.0f; // 一點誤差

    return (m_Transform.translation.y >= bottomBound - epsilon ||
            m_Transform.translation.y <= topBound + epsilon);
}