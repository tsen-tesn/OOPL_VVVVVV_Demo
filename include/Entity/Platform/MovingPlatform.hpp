#ifndef MOVINGPLATFORM_HPP
#define MOVINGPLATFORM_HPP

#include "Entity/Platform/Platform.hpp"
#include "Util/Image.hpp"
#include <memory>

class MovingPlatform : public Platform {
public:
    MovingPlatform(const glm::vec2& OriginalPos, const glm::vec2& StartPos, const glm::vec2& EndPos, const std::string& imagePath,float scale = 3.0f, float speed =100.0f);

    void Update() override;
    glm::vec2 GetDelta() const override { return m_Delta; }

private:
    glm::vec2 m_StartPos;
    glm::vec2 m_EndPos;
    glm::vec2 m_OriginalPos;
    bool m_MovingToEnd = true;
    float m_Speed = 100.0f;
    glm::vec2 m_Delta = {0.0f, 0.0f};
};

#endif // MOVINGPLATFORM_HPP