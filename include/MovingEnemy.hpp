#ifndef MOVINGENEMY_HPP
#define MOVINGENEMY_HPP

#include <string>

#include "Hazard.hpp"
#include "Util/GameObject.hpp"

class MovingEnemy : public Hazard {
public:
    MovingEnemy(const glm::vec2& position, const std::string& imagePath);     // MovingEnemy constructor 接收位置和圖片路徑 
    ~MovingEnemy() override = default;

    void Update() override;

    bool is_touched(const glm::vec2 &playerPos) const override;
};

#endif // MOVINGENEMY_HPP