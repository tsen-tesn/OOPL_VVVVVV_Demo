#ifndef SCENE_HPP
#define SCENE_HPP

#include <optional>

enum class SceneType {
    Menu,
    Game,
    Pause,
};

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual bool ShouldQuit() const = 0;
    virtual bool ShouldPause() const = 0;
    virtual std::optional<SceneType> GetNextScene() const { return std::nullopt; }
};

#endif // SCENE_HPP
