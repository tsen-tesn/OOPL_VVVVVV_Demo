#ifndef HAZARD_HPP
#define HAZARD_HPP

#include <glm/vec2.hpp>

#include "Util/GameObject.hpp"

class Hazard : public Util::GameObject {
public:
    virtual ~Hazard() = default;

    virtual bool is_touched(const glm::vec2 &playerPos) const = 0;
};

#endif 