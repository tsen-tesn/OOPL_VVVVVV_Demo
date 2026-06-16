#ifndef SCENE_HPP
#define SCENE_HPP

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Update() = 0;
    virtual bool ShouldQuit() const = 0;
    virtual bool ShouldPause() const = 0;
};

#endif // SCENE_HPP
