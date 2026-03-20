#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>

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

private:
    std::shared_ptr<Util::Animation> m_RightDownAnimation;
    std::shared_ptr<Util::Animation> m_LeftDownAnimation;
    std::shared_ptr<Util::Animation> m_RightUpAnimation;
    std::shared_ptr<Util::Animation> m_LeftUpAnimation;

    glm::vec2 m_Velocity = {0, 0};
    float m_Gravity = 500.0f;
    bool m_GravityDown = true;
    bool m_FacingRight = true;
    bool is_grounded();
};

#endif