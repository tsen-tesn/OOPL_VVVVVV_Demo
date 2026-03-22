#include <cmath>

#include "Spike.hpp"
#include "Util/Image.hpp"

Spike::Spike(const glm::vec2& position, const std::string& imagePath) {
    m_Transform.translation = position;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    m_Transform.scale = {3.0f, 3.0f};
}

bool Spike::is_touched(const glm::vec2 &playerPos) const {
    glm::vec2 spikePos = m_Transform.translation;

    float playerHalfSize = 12.0f * 3.0f;  // 玩家半寬（自己調）
    float spikeHalfSize  = 4.0f * 3.0f ;  // Spike 半寬（自己調）

    float playerLeft   = playerPos.x - playerHalfSize;
    float playerRight  = playerPos.x + playerHalfSize;
    float playerTop    = playerPos.y - playerHalfSize;
    float playerBottom = playerPos.y + playerHalfSize;

    float spikeLeft   = spikePos.x - spikeHalfSize;
    float spikeRight  = spikePos.x + spikeHalfSize;
    float spikeTop    = spikePos.y - spikeHalfSize;
    float spikeBottom = spikePos.y + spikeHalfSize;

    return (playerRight  > spikeLeft &&
            playerLeft   < spikeRight &&
            playerBottom > spikeTop &&
            playerTop    < spikeBottom);
}