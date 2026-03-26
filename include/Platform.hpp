#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Util/GameObject.hpp"

class Platform : public Util::GameObject {
public:
    Platform() = default;

    virtual void Update() {}

    virtual bool IsSolid() const { return true; }
};

#endif // PLATFORM_HPP