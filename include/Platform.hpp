#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Util/GameObject.hpp"

class Platform : public Util::GameObject {
public:
    Platform() = default;

    virtual ~Platform() = default;

    virtual void Update() {}

    virtual void Draw() { Util::GameObject::Draw(); }

    virtual bool IsSolid() const { return true; }

    virtual glm::vec2 GetPosition() const { return m_Transform.translation; }

    virtual void Disappear() {}

    virtual bool IsDisappeared() const { return false; }
};

#endif // PLATFORM_HPP