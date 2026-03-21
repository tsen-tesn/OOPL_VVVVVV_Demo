#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"

void App::Start() {
    LOG_TRACE("Start");
    // for testing
    m_Player  = std::make_shared<Player>();

    m_Level = std::make_shared<LoadLevel>(
        RESOURCE_DIR "/Map/VVVVVV Demo/room1.json"
    );
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    m_Level->Draw();    // background first
    m_Player->Update();
    m_Player->Draw();
    
    for (const auto& hazard : m_Level->GetHazards()) {
        hazard->Update();
        if (hazard->is_touched(m_Player->GetPosition())) {
            m_Player->Die();
            LOG_INFO("Player died");
        }
    }
    
    // Simple Room Transition Logic
    glm::vec2 pos = m_Player->GetTransform().translation;
    auto conn = m_Level->GetConnections();
    float halfW = 540.0f; // match Player bounds
    float halfH = 330.0f;

    

    // Right Edge
    if (pos.x >= halfW) {
        if (conn.right != -1) {
            try {
                m_Level = std::make_shared<LoadLevel>(RESOURCE_DIR "/Map/VVVVVV Demo/room" + std::to_string(conn.right) + ".json");
                pos.x = -halfW + 10.0f; // wrap to left side
            } catch(const std::exception& e) {
                LOG_ERROR("Failed to load right connection: {}", e.what());
                pos.x = halfW; // block
            }
        } else {
            pos.x = halfW; // block
        }
    }
    // Left Edge
    else if (pos.x <= -halfW) {
        if (conn.left != -1) {
            try {
                m_Level = std::make_shared<LoadLevel>(RESOURCE_DIR "/Map/VVVVVV Demo/room" + std::to_string(conn.left) + ".json");
                pos.x = halfW - 10.0f; // wrap to right side
            } catch(const std::exception& e) {
                LOG_ERROR("Failed to load left connection: {}", e.what());
                pos.x = -halfW; // block
            }
        } else {
            pos.x = -halfW; // block
        }
    }
    // Top Edge
    else if (pos.y >= halfH) {
        if (conn.up != -1) {
            try {
                m_Level = std::make_shared<LoadLevel>(RESOURCE_DIR "/Map/VVVVVV Demo/room" + std::to_string(conn.up) + ".json");
                pos.y = -halfH + 10.0f; // wrap to bottom
            } catch(const std::exception& e) {
                LOG_ERROR("Failed to load up connection: {}", e.what());
                pos.y = halfH; // block
            }
        } else {
            pos.y = halfH; // block
        }
    }
    // Bottom Edge
    else if (pos.y <= -halfH) {
        if (conn.down != -1) {
            try {
                m_Level = std::make_shared<LoadLevel>(RESOURCE_DIR "/Map/VVVVVV Demo/room" + std::to_string(conn.down) + ".json");
                pos.y = halfH - 10.0f; // wrap to top
            } catch(const std::exception& e) {
                LOG_ERROR("Failed to load down connection: {}", e.what());
                pos.y = -halfH; // block
            }
        } else {
            pos.y = -halfH; // block
        }
    }

    if (pos != m_Player->GetTransform().translation) {
        m_Player->m_Transform.translation = pos;
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
