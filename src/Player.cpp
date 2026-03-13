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
    // 假設分組如下：
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

    // 上下方向切換：只在按下當下切換，並清掉原本垂直速度
    // 這裡沿用你前面實測過比較正確的對應
    if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
        m_GravityDown = true;
        m_Velocity.y = 0.0f;
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        m_GravityDown = false;
        m_Velocity.y = 0.0f;
    }

    // 套用重力
    float gravityDirection = m_GravityDown ? 1.0f : -1.0f;
    m_Velocity.y += m_Gravity * gravityDirection * deltaTime;

    // 更新位置
    m_Transform.translation += m_Velocity * deltaTime;

    // 根據狀態切換動畫
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

    // 因為素材本身已經分上下左右，所以這裡不要再用負 scale 去翻
    m_Transform.scale = {2.0f, 2.0f};

    // 畫面邊界
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