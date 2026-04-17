#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include <glm/vec2.hpp>
#include "Util/GameObject.hpp"

class Trigger : public Util::GameObject {
public:
    virtual ~Trigger() = default;
    virtual bool is_touched(const glm::vec2 &playerPos) const = 0;
    virtual void Update() {};
};

#endif