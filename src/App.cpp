#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Renderer.hpp"

void App::Start() {
    LOG_TRACE("Start");
    // for testing
    m_Player = std::make_shared<Player>();
    m_Spike1 = std::make_shared<Spike>(glm::vec2{100.0f, -300.0f}, "Resources/tile_6.png");
    m_Spike2 = std::make_shared<Spike>(glm::vec2{116.0f, -300.0f}, "Resources/tile_6.png");
    m_Spike3 = std::make_shared<Spike>(glm::vec2{132.0f, -300.0f}, "Resources/tile_6.png");
    m_Spike4 = std::make_shared<Spike>(glm::vec2{148.0f, -300.0f}, "Resources/tile_6.png");
    m_Spike5 = std::make_shared<Spike>(glm::vec2{164.0f, -300.0f}, "Resources/tile_6.png");

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    m_Player->Update();
    m_Player->Draw();
    m_Spike1->Draw();
    m_Spike2->Draw();
    m_Spike3->Draw();
    m_Spike4->Draw();
    m_Spike5->Draw();

    // for testing
    if (m_Spike1 && m_Player && m_Spike1->is_touched(m_Player->GetPosition())) {
        m_Player->Die();
        LOG_INFO("Player is died");
    }
    if (m_Spike2 && m_Player && m_Spike2->is_touched(m_Player->GetPosition())) {
        m_Player->Die();
        LOG_INFO("Player is died");
    }
    if (m_Spike3 && m_Player && m_Spike3->is_touched(m_Player->GetPosition())) {
        m_Player->Die();
        LOG_INFO("Player is died");
    }
    if (m_Spike4 && m_Player && m_Spike4->is_touched(m_Player->GetPosition())) {
        m_Player->Die();
        LOG_INFO("Player is died");
    }
    if (m_Spike5 && m_Player && m_Spike5->is_touched(m_Player->GetPosition())) {
        m_Player->Die();
        LOG_INFO("Player is died");
    }

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
