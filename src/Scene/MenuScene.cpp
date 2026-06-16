#include "MenuScene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

void MenuScene::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        m_NextScene = SceneType::Game;
        return;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_ShouldQuit = true;
    }
}

void MenuScene::Draw() {
}
