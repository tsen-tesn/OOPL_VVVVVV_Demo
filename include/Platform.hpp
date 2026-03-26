#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Util/GameObject.hpp"

class Platform : public Util::GameObject {
public:
    Platform();

    virtual void Update();

    virtual bool IsSolid() const;
};

#endif // PLATFORM_HPP