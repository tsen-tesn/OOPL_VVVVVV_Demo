#ifndef DISAPPEARINGPLATFORMGROUP_HPP
#define DISAPPEARINGPLATFORMGROUP_HPP

#include "Entity/Platform/DisappearingPlatform.hpp"
#include <memory>
#include <vector>

class DisappearingPlatformGroup : public Platform {
public:
    DisappearingPlatformGroup(const std::vector<glm::vec2>& positions, const std::vector<std::string>& imagePaths, float scale = 3.0f);
    
    void Update() override;
    void Draw() override;
    void Disappear() override;
    void Reset() override;
    bool IsDisappeared() const override;
    bool IsSolid() const override;

    void CheckCollisionAndDisappear(const glm::vec2& playerPosition);
    
    const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const { return m_Platforms; }

private:
    std::vector<std::shared_ptr<Platform>> m_Platforms;
};

#endif // DISAPPEARINGPLATFORMGROUP_HPP
