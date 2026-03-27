#include "DisappearingPlatformGroup.hpp"
#include "Util/Logger.hpp"

DisappearingPlatformGroup::DisappearingPlatformGroup(const std::vector<glm::vec2>& positions, const std::vector<std::string>& imagePaths, float scale) {
    for (size_t i = 0; i < positions.size(); ++i) {
        m_Platforms.push_back(std::make_shared<DisappearingPlatform>(positions[i], imagePaths, scale));
    }
    // LOG_INFO("DisappearingPlatformGroup created with {} platforms", m_Platforms.size());
}

void DisappearingPlatformGroup::CheckCollisionAndDisappear(const std::shared_ptr<Player>& player) {
    // 如果組已經開始消失，不再檢查
    bool alreadyTriggered = false;
    for (auto& platform : m_Platforms) {
        if (platform->IsDisappeared()) {
            alreadyTriggered = true;
            break;
        }
    }
    if (alreadyTriggered) return;
    
    glm::vec2 playerPos = player->GetPosition();
    for (auto& platform : m_Platforms) {
        glm::vec2 platformPos = platform->GetPosition();
        float distance = glm::distance(playerPos, platformPos);
        if (distance < 72.0f) {
            for (auto& p : m_Platforms) {
                p->Disappear();
            }
            LOG_INFO("DisappearingPlatformGroup triggered disappearance");
            break;
        }
    }
}

void DisappearingPlatformGroup::Update() {
    for (auto& platform : m_Platforms) {
        platform->Update();
    }
}

void DisappearingPlatformGroup::Draw() {
    for (auto& platform : m_Platforms) {
        platform->Draw();
    }
}

void DisappearingPlatformGroup::Disappear() {
    for (auto& platform : m_Platforms) {
        platform->Disappear();
    }
}

bool DisappearingPlatformGroup::IsDisappeared() const {
    for (auto& platform : m_Platforms) {
        if (!platform->IsDisappeared()) return false;
    }
    return true;
}

bool DisappearingPlatformGroup::IsSolid() const {
    for (auto& platform : m_Platforms) {
        if (!platform->IsDisappeared() && platform->IsSolid()) return true;
    }
    return false;
}