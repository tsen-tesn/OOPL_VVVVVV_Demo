#include "Entity/Trigger/CheckPoint.hpp"
#include "Entity/Collision.hpp"

CheckPoint::CheckPoint(const glm::vec2& position, const std::string& imagePath) {
    m_Transform.translation = position;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    m_Transform.scale = {3.2f, 3.2f};
}

bool CheckPoint::IsTouched(const glm::vec2& playerPos) const {
    return Collision::Overlaps(Collision::PlayerRect(playerPos), Collision::ObjectRect(*this));
}

void CheckPoint::SetActivated(bool activated) {
    m_isActivated = activated;

    // 之後要做換圖

}
