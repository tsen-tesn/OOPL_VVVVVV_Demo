#ifndef SPIKE_HPP
#define SPIKE_HPP

#include <string>

#include "Hazard.hpp"
#include "Util/GameObject.hpp"

class Spike : public Hazard, public Util::GameObject {
public:
    Spike(const glm::vec2& position, const std::string& imagePath);     // Spike constructor 接收位置和圖片路徑 
    ~Spike() override = default;

    bool is_touched(const glm::vec2 &playerPos) const override;
};

#endif // SPIKE_HPP