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
    m_Hazards.push_back(std::make_shared<Spike>(glm::vec2{100.0f, -300.0f}, "Resources/tile_6.png"));
    m_Hazards.push_back(std::make_shared<Spike>(glm::vec2{116.0f, -300.0f}, "Resources/tile_6.png"));
    m_Hazards.push_back(std::make_shared<Spike>(glm::vec2{132.0f, -300.0f}, "Resources/tile_6.png"));
    m_Hazards.push_back(std::make_shared<Spike>(glm::vec2{148.0f, -300.0f}, "Resources/tile_6.png"));
    m_Hazards.push_back(std::make_shared<Spike>(glm::vec2{164.0f, -300.0f}, "Resources/tile_6.png"));

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    m_Player->Update();
    m_Player->Draw();
    
    for (const auto& hazard : m_Hazards) {
        hazard->Draw();

        if (hazard->is_touched(m_Player->GetPosition())) {
            m_Player->Die();
            LOG_INFO("Player died");
        }
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
