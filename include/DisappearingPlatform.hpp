#ifndef DISAPPEARINGPLATFORM_HPP
#define DISAPPEARINGPLATFORM_HPP

#include "Platform.hpp"
#include "Util/Image.hpp"
#include <memory>

class DisappearingPlatform : public Platform {
public:
    DisappearingPlatform(const glm::vec2& Pos, const std::vector<std::string>& imagePaths, float scale = 3.0f);

    void Update() override;

private:
    glm::vec2 m_Pos;
};

#endif // DISAPPEARINGPLATFORM_HPP