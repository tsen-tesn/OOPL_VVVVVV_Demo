#ifndef MENUSCENE_HPP
#define MENUSCENE_HPP

#include <memory>
#include <optional>
#include <vector>

#include "Scene.hpp"
#include "Util/GameObject.hpp"

class MenuScene : public Scene {
public:
    MenuScene();

    void Update() override;
    void Draw() override;

    bool ShouldQuit() const override { return m_ShouldQuit; }
    std::optional<SceneType> GetNextScene() const override { return m_NextScene; }

private:
    bool IsEnterPressed() const;

private:
    bool m_ShouldQuit = false;
    std::optional<SceneType> m_NextScene;
    std::vector<std::shared_ptr<Util::GameObject>> m_TextObjects;
    std::shared_ptr<Util::GameObject> m_StartPrompt;
};

#endif // MENUSCENE_HPP
