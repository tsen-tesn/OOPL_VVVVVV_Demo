#include "App.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    try {
        m_GameScene = std::make_shared<GameScene>();
        m_CurrentState = State::UPDATE;
    } catch (const std::exception& e) {
        LOG_ERROR("App::Start failed: {}", e.what());
        m_CurrentState = State::END;
    }
}

void App::Update() {
    switch (m_CurrentState) {
        case State::UPDATE:
            m_GameScene->Update();
            if (m_GameScene->ShouldQuit())
                m_CurrentState = State::END;
            else if (m_GameScene->ShouldPause())
                m_CurrentState = State::PAUSE; // Phase 4 預留
            break;

        case State::PAUSE:
            // Phase 4：PauseScene 在此處理
            // 暫時按 P 繼續
            m_CurrentState = State::UPDATE;
            break;

        case State::START:
            // Phase 4：StartScene 在此處理
            m_CurrentState = State::UPDATE;
            break;

        case State::END:
        default:
            break;
    }
}

void App::End() {
    LOG_TRACE("End");
}
