#ifndef MOVINGENEMY_HPP
#define MOVINGENEMY_HPP

#include <string>
#include <vector>

#include "Hazard.hpp"
#include "Util/Animation.hpp"

class MovingEnemy : public Hazard {
public:
    // 要給定 MovingEnemy 兩點移動的位置 以及動畫圖片路徑
    MovingEnemy(const glm::vec2& startPos, const glm::vec2& endPos, const std::vector<std::string>& imagePaths);

    ~MovingEnemy() override = default;

    void Update() override;
    bool is_touched(const glm::vec2& playerPos) const override;

private:
    glm::vec2 m_StartPos;
    glm::vec2 m_EndPos;
    bool m_MovingToEnd = true;
    float m_Speed = 100.0f;
};

#endif // MOVINGENEMY_HPP