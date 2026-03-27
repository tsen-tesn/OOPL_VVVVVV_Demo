#include "DisappearingPlatform.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

DisappearingPlatform::DisappearingPlatform(const glm::vec2& Pos, const std::vector<std::string>& imagePaths, float scale) : m_Pos(Pos), m_ImagePaths(imagePaths) {
    m_NormalImage = std::make_shared<Util::Image>(m_ImagePaths[0]);

    // 假設第二個參數是是否循環，這裡應該用 false
    m_DisappearAnimation = std::make_shared<Util::Animation>(m_ImagePaths, false, 200, true);

    m_Drawable = m_NormalImage;
    m_Transform.translation = m_Pos;
    m_Transform.scale = {scale, scale};
    m_Visible = true;

    m_AnimationDuration = static_cast<float>(m_ImagePaths.size()) * 0.2f;

    // LOG_INFO("DisappearingPlatform created");
}

void DisappearingPlatform::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // 先不要急著呼叫 Platform::Update()
    // 先確認是不是它覆蓋掉你的動畫
    // Platform::Update();

    if (m_State == State::NORMAL) {
        m_Drawable = m_NormalImage;
        m_Visible = true;
    }
    else if (m_State == State::ANIMATING) {
        m_Drawable = m_DisappearAnimation;
        m_Visible = true;
        m_AnimationTimer += deltaTime;

        if (m_AnimationTimer >= m_AnimationDuration) {
            m_State = State::DISAPPEARED;
            m_Visible = false;
            // LOG_INFO("DisappearingPlatform animation finished, disappeared");
        }
    }
    else if (m_State == State::DISAPPEARED) {
        m_Visible = false;
    }
}

void DisappearingPlatform::Disappear() {
    if (m_State == State::NORMAL) {
        m_State = State::ANIMATING;
        m_AnimationTimer = 0.0f;

        // 重新建立動畫，讓它從第一幀開始
        m_DisappearAnimation = std::make_shared<Util::Animation>(m_ImagePaths, true, 200, true);

        LOG_INFO("DisappearingPlatform started disappearing animation");
    }
}

bool DisappearingPlatform::IsSolid() const {
    return m_State != State::DISAPPEARED && Platform::IsSolid();
}