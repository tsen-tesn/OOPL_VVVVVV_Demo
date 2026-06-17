#include "MenuScene.hpp"

#include "Util/Color.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Text.hpp"

namespace {
const std::string kFont = "Resources/Font/space-station.ttf";

std::shared_ptr<Util::GameObject> CreateTextObject(
    const std::string& text,
    int size,
    const glm::vec2& position,
    const Util::Color& color) {
    auto object = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(kFont, size, text, color),
        10.0f
    );
    object->m_Transform.translation = position;
    return object;
}

std::shared_ptr<Util::GameObject> CreateImageObject(
    const std::string& imagePath,
    const glm::vec2& position,
    const glm::vec2& scale) {
    auto object = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(imagePath),
        10.0f
    );
    object->m_Transform.translation = position;
    object->m_Transform.scale = scale;
    return object;
}
} // namespace

MenuScene::MenuScene() {
    const auto titleColor = Util::Color::FromRGB(120, 120, 120);
    const auto promptColor = Util::Color::FromRGB(150, 150, 150);

    constexpr int kTitleLetterCount = 6;
    constexpr float kTitleStartX = -215.0f;
    constexpr float kTitleSpacing = 86.0f;
    for (int i = 0; i < kTitleLetterCount; ++i) {
        m_TextObjects.push_back(CreateImageObject(
            "Resources/Font/V.png",
            {kTitleStartX + kTitleSpacing * i, 135.0f},
            {3.0f, 3.0f}
        ));
    }

    m_TextObjects.push_back(CreateTextObject("Demo", 24, {235.0f, 80.0f}, titleColor));

    m_StartPrompt = CreateTextObject("[ Press Enter to Start ]", 28, {0.0f, -120.0f}, promptColor);
    m_TextObjects.push_back(m_StartPrompt);
}

void MenuScene::Update() {
    if (IsEnterPressed()) {
        m_NextScene = SceneType::Game;
        return;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_ShouldQuit = true;
    }
}

void MenuScene::Draw() {
    for (const auto& textObject : m_TextObjects) {
        textObject->Draw();
    }
}

bool MenuScene::IsEnterPressed() const {
    return Util::Input::IsKeyDown(Util::Keycode::RETURN);
}
