#include "Entity/Hazard/MovingEnemy.hpp"

#include <cmath>
#include <memory>

#include "Entity/Collision.hpp"
#include "Util/Time.hpp"

MovingEnemy::MovingEnemy(const glm::vec2& startPos,
                         const glm::vec2& endPos,
                         const std::vector<std::string>& imagePaths,
                         float scale,
                         float speed)
    : m_StartPos(startPos), m_EndPos(endPos), m_Speed(speed) {
    m_Transform.translation = startPos;
    m_Transform.scale       = {scale, scale};
    m_Drawable              = std::make_shared<Util::Animation>(imagePaths, true, 100, true);
}

void MovingEnemy::Update() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    const glm::vec2 target    = m_MovingToEnd ? m_EndPos : m_StartPos;
    const glm::vec2 direction = target - m_Transform.translation;
    const float     distance  = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    const float     moveDist  = m_Speed * deltaTime;

    if (distance <= moveDist) {
        m_Transform.translation = target;
        m_MovingToEnd = !m_MovingToEnd;
    } else {
        m_Transform.translation += (direction / distance) * moveDist;
    }
}

bool MovingEnemy::is_touched(const glm::vec2& playerPos) const {
    return Collision::Overlaps(Collision::PlayerRect(playerPos), Collision::ObjectRect(*this));
}
