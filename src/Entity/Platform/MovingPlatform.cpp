#include "Entity/Platform/MovingPlatform.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

MovingPlatform::MovingPlatform(const glm::vec2& OriginalPos, const glm::vec2& StartPos, const glm::vec2& EndPos, const std::string& imagePath, float scale, float speed) : m_StartPos(StartPos), m_EndPos(EndPos), m_OriginalPos(OriginalPos), m_Speed(speed) {
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    
    m_Transform.translation = OriginalPos;
    m_Transform.scale = {scale, scale};
    // LOG_INFO("MovingPlatform created");
}

void MovingPlatform::Update() {
    Platform::Update();
    glm::vec2 prevPos = m_Transform.translation;
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

    m_Delta = m_Transform.translation - prevPos;
}