#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include "pch.hpp" // IWYU pragma: export
#include "Scene.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        PAUSE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    void ChangeScene(SceneType sceneType);
    std::shared_ptr<Scene> CreateScene(SceneType sceneType);

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Scene> m_CurrentScene;
};

#endif
