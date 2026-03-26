#include "DisappearingPlatform.hpp"
#include "Util/Logger.hpp"

DisappearingPlatform::DisappearingPlatform(const glm::vec2& Pos, const std::vector<std::string>& imagePaths, float scale): m_Pos(Pos) {
    m_Drawable = std::make_shared<Util::Image>(imagePaths[0]);
    m_Transform.translation = m_Pos;
    m_Transform.scale = {scale, scale};
    LOG_INFO("DisappearingPlatform created");
}

void DisappearingPlatform::Update() {
    Platform::Update();
    LOG_INFO("DisappearingPlatform updated");
}