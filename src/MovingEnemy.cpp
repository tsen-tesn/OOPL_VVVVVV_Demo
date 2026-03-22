#include "MovingEnemy.hpp"

#include <cmath>
#include <memory>

#include "Util/Time.hpp"

MovingEnemy::MovingEnemy(const glm::vec2& startPos, const glm::vec2& endPos, const std::vector<std::string>& imagePaths, float scale, float speed) : m_StartPos(startPos), m_EndPos(endPos) {
    m_Transform.translation = startPos;
    m_Transform.scale = {scale, scale};
    m_Speed = speed;
    m_Drawable = std::make_shared<Util::Animation>(imagePaths, true, 100, true);
}

void MovingEnemy::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    glm::vec2 target = m_MovingToEnd ? m_EndPos : m_StartPos;
    glm::vec2 direction = target - m_Transform.translation;

    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    float moveDist = m_Speed * deltaTime;

    if (distance <= moveDist) {
        m_Transform.translation = target;
        m_MovingToEnd = !m_MovingToEnd;
    } else {
        direction /= distance;
        m_Transform.translation += direction * moveDist;
    }
}

bool MovingEnemy::is_touched(const glm::vec2& playerPos) const {
    glm::vec2 enemyPos = m_Transform.translation;

    float playerHalfSize = 16.0f;
    float enemyHalfSize = 16.0f;

    float playerLeft   = playerPos.x - playerHalfSize;
    float playerRight  = playerPos.x + playerHalfSize;
    float playerTop    = playerPos.y - playerHalfSize;
    float playerBottom = playerPos.y + playerHalfSize;

    float enemyLeft   = enemyPos.x - enemyHalfSize;
    float enemyRight  = enemyPos.x + enemyHalfSize;
    float enemyTop    = enemyPos.y - enemyHalfSize;
    float enemyBottom = enemyPos.y + enemyHalfSize;

    return (playerRight  > enemyLeft &&
            playerLeft   < enemyRight &&
            playerBottom > enemyTop &&
            playerTop    < enemyBottom);
}