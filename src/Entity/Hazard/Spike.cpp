#include "Spike.hpp"
#include "Entity/Collision.hpp"
#include "Util/Image.hpp"

Spike::Spike(const glm::vec2& position, const std::string& imagePath) {
    m_Transform.translation = position;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    m_Transform.scale = {3.0f, 3.0f};
}

bool Spike::is_touched(const glm::vec2& playerPos) const {
    return Collision::Overlaps(Collision::PlayerRect(playerPos), Collision::ObjectRect(*this));
}
