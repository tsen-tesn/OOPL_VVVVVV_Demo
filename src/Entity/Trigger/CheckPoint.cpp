#include "Entity/Trigger/CheckPoint.hpp"

CheckPoint::CheckPoint(const glm::vec2& position, const std::string& imagePath) {
    m_Transform.translation = position;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    m_Transform.scale = {3.2f, 3.2f};
}

bool CheckPoint::is_touched(const glm::vec2& playerPos) const {
    glm::vec2 checkpointPos = m_Transform.translation;

    float playerHalfSize = 16.0f;
    float checkpointHalfSize = 16.0f;

    float playerLeft   = playerPos.x - playerHalfSize;
    float playerRight  = playerPos.x + playerHalfSize;
    float playerTop    = playerPos.y - playerHalfSize;
    float playerBottom = playerPos.y + playerHalfSize;

    float checkpointLeft   = checkpointPos.x - checkpointHalfSize;
    float checkpointRight  = checkpointPos.x + checkpointHalfSize;
    float checkpointTop    = checkpointPos.y - checkpointHalfSize;
    float checkpointBottom = checkpointPos.y + checkpointHalfSize;

    return (playerRight  > checkpointLeft &&
            playerLeft   < checkpointRight &&
            playerBottom > checkpointTop &&
            playerTop    < checkpointBottom);
}

void CheckPoint::SetActivated(bool activated) {
    m_isActivated = activated;

    // 之後要做換圖

}