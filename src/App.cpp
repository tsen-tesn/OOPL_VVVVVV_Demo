#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Renderer.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_Player = std::make_shared<Player>();
    // m_Spike = std::make_shared<Spike>();
    // m_Spike->m_Transform.translation = {100.0f, -300.0f}; // tile_6.png 位置範例
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    m_Player->Update();
    m_Player->Draw();
    m_Spike->Draw();

    // if (m_Spike && m_Player && m_Spike->is_touched(m_Player->GetPosition())) {
    //     LOG_INFO("Player is died");
    // }

    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
