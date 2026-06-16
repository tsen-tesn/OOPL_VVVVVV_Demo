#ifndef MENUSCENE_HPP
#define MENUSCENE_HPP

#include <optional>

#include "Scene.hpp"

class MenuScene : public Scene {
public:
    void Update() override;
    void Draw() override;

    bool ShouldQuit() const override { return m_ShouldQuit; }
    bool ShouldPause() const override { return false; }
    std::optional<SceneType> GetNextScene() const override { return m_NextScene; }

private:
    bool m_ShouldQuit = false;
    std::optional<SceneType> m_NextScene;
};

#endif // MENUSCENE_HPP
