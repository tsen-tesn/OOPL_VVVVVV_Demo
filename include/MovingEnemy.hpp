#ifndef MOVINGENEMY_HPP
#define MOVINGENEMY_HPP

#include <string>
<<<<<<< HEAD
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
=======

#include "Hazard.hpp"
#include "Util/GameObject.hpp"

class MovingEnemy : public Hazard {
public:
    MovingEnemy(const glm::vec2& position, const std::string& imagePath);     // MovingEnemy constructor 接收位置和圖片路徑 
    ~MovingEnemy() override = default;

    void Update() override;

    bool is_touched(const glm::vec2 &playerPos) const override;
>>>>>>> bc26b60 (refactor: add virtual Update method to Hazard for unified behavior handling)
};

#endif // MOVINGENEMY_HPP