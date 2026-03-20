#include "Player.hpp"

#include <memory>
#include <string>
#include <vector>

namespace {
    std::shared_ptr<Util::Animation> CreateAnimation(int start, int end) {
        std::vector<std::string> frames;
        for (int i = start; i <= end; ++i) {
            frames.push_back("Resources/character/main/role_" + std::to_string(i) + ".png");
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

    // 預設狀態：面右、往下
    m_Drawable = m_RightDownAnimation;
    m_ZIndex = 1.0f;
    m_Transform.scale = {2.0f, 2.0f};
}

void Player::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
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


    if (is_grounded()) {
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

    m_Transform.scale = {2.0f, 2.0f};

    const float leftBound   = -400.0f;
    const float rightBound  =  400.0f;
    const float topBound    = -300.0f;
    const float bottomBound =  300.0f;

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

bool Player::is_grounded() {
    const float topBound = -300.0f;
    const float bottomBound = 300.0f;
    const float epsilon = 1.0f; // 一點誤差

    return (m_Transform.translation.y >= bottomBound - epsilon ||
            m_Transform.translation.y <= topBound + epsilon);
}