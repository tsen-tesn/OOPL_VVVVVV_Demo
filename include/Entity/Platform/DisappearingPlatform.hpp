#ifndef DISAPPEARINGPLATFORM_HPP
#define DISAPPEARINGPLATFORM_HPP

#include "Platform.hpp"
#include "Util/Image.hpp"
#include "Util/Animation.hpp"
#include <memory>

class DisappearingPlatform : public Platform {
public:
    enum class State {
        NORMAL,
        ANIMATING,
        DISAPPEARED
    };

    DisappearingPlatform(const glm::vec2& Pos, const std::vector<std::string>& imagePaths, float scale = 3.0f);

    void Update() override;
    void Disappear() override;
    bool IsDisappeared() const override { return m_State == State::DISAPPEARED; }
    bool IsSolid() const override;
    glm::vec2 GetPosition() const override { return m_Transform.translation; }

private:
    State m_State = State::NORMAL;
    std::shared_ptr<Util::Image> m_NormalImage;
    std::vector<std::string> m_ImagePaths;
    std::shared_ptr<Util::Animation> m_DisappearAnimation;
    float m_AnimationTimer = 0.0f;
    float m_AnimationDuration = 1.0f; // 動畫持續時間
    glm::vec2 m_Pos;
};

#endif // DISAPPEARINGPLATFORM_HPP