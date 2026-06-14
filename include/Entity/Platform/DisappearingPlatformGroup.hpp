#ifndef DISAPPEARINGPLATFORMGROUP_HPP
#define DISAPPEARINGPLATFORMGROUP_HPP

#include "Entity/Platform/DisappearingPlatform.hpp"
#include "Entity/Player.hpp"
#include <memory>
#include <vector>

class DisappearingPlatformGroup : public Platform {
public:
    DisappearingPlatformGroup(const std::vector<glm::vec2>& positions, const std::vector<std::string>& imagePaths, float scale = 3.0f);
    
    void Update() override;
    void Draw() override;
    void Disappear() override;
    bool IsDisappeared() const override;
    bool IsSolid() const override;

    void CheckCollisionAndDisappear(const std::shared_ptr<Player>& player);
    
    const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const { return m_Platforms; }

private:
    std::vector<std::shared_ptr<Platform>> m_Platforms;
};

#endif // DISAPPEARINGPLATFORMGROUP_HPP