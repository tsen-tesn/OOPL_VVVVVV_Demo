#include "CheckPoint.hpp"

CheckPoint::CheckPoint(const glm::vec2& position, const std::string& imagePath) {
    m_Transform.translation = position;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
    m_Transform.scale = {3.2f, 3.2f};
}

void CheckPoint::Update() {
    
}

bool CheckPoint::is_touched(const glm::vec2& playerPos) const {
    
}
