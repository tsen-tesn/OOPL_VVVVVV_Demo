#include "App.hpp"
#include "GameScene.hpp"
#include "MenuScene.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    try {
        ChangeScene(SceneType::Menu);
        m_CurrentState = State::UPDATE;
    } catch (const std::exception& e) {
        LOG_ERROR("App::Start failed: {}", e.what());
        m_CurrentState = State::END;
    }
}

void App::Update() {
    switch (m_CurrentState) {
        case State::UPDATE:
            m_CurrentScene->Update();
            m_CurrentScene->Draw();
            if (m_CurrentScene->ShouldQuit())
                m_CurrentState = State::END;
            else if (const auto nextScene = m_CurrentScene->GetNextScene())
                ChangeScene(*nextScene);
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

void App::ChangeScene(SceneType sceneType) {
    m_CurrentScene = CreateScene(sceneType);
    if (!m_CurrentScene) {
        LOG_ERROR("Failed to create scene");
        m_CurrentState = State::END;
    }
}

std::shared_ptr<Scene> App::CreateScene(SceneType sceneType) {
    switch (sceneType) {
        case SceneType::Menu:
            return std::make_shared<MenuScene>();
        case SceneType::Game:
            return std::make_shared<GameScene>();
    }

    return nullptr;
}
