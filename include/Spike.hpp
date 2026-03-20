#ifndef SPIKE_HPP
#define SPIKE_HPP

#include "Hazard.hpp"
#include "Util/GameObject.hpp"

class Spike : public Hazard, public Util::GameObject{
public:
    Spike();
    ~Spike() override = default;

    bool is_touched(const glm::vec2 &playerPos) const override;
};

#endif // SPIKE_HPP
