#include <cmath>

#include "Spike.hpp"
#include "Util/Image.hpp"

Spike::Spike() {
    m_Drawable = std::make_shared<Util::Image>("Resources/tile_6.png"); 
    m_Transform.scale = {2.0f, 2.0f};
}

bool Spike::is_touched(const glm::vec2 &playerPos) const {
    const glm::vec2 spikePos = m_Transform.translation;
    const float thresholdX = 30.0f;
    const float thresholdY = 30.0f;

    return std::abs(playerPos.x - spikePos.x) <= thresholdX &&
           std::abs(playerPos.y - spikePos.y) <= thresholdY;
}
