#ifndef SCENE_HPP
#define SCENE_HPP

/// 場景介面：所有 Scene 都須實作 Update()
class Scene {
public:
    virtual ~Scene() = default;
    virtual void Update() = 0;
};

#endif // SCENE_HPP
