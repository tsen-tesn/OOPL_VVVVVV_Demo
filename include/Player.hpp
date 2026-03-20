#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

class Player : public Util::GameObject {
public:
    Player();
    void Update();

    // 取得即時位置，用於碰撞判定
    glm::vec2 GetPosition() const;

    void Die();
    bool IsDead() const;

private:
    std::shared_ptr<Util::Animation> m_RightDownAnimation;
    std::shared_ptr<Util::Animation> m_LeftDownAnimation;
    std::shared_ptr<Util::Animation> m_RightUpAnimation;
    std::shared_ptr<Util::Animation> m_LeftUpAnimation;
    std::shared_ptr<Util::Image> m_RightDownImage;
    std::shared_ptr<Util::Image> m_LeftDownImage;
    std::shared_ptr<Util::Image> m_RightUpImage;
    std::shared_ptr<Util::Image> m_LeftUpImage;
    std::shared_ptr<Util::Image> m_RightDownDeathImage;
    std::shared_ptr<Util::Image> m_LeftDownDeathImage;
    std::shared_ptr<Util::Image> m_RightUpDeathImage;
    std::shared_ptr<Util::Image> m_LeftUpDeathImage;

    glm::vec2 m_Velocity = {0, 0};
    float m_Gravity = 500.0f;
    bool m_GravityDown = true;
    bool m_FacingRight = true;
    bool IsGrounded();

    bool m_IsDead = false;
    float m_DeathTimer = 0.0f;
    float m_DeathDuration = 1.0f;   // 閃爍總長度，可自己調
    float m_BlinkInterval = 0.05f;  // 每 0.05 秒切換一次顯示/隱藏
};

#endif