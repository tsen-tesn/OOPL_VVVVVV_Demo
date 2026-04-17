#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "TileMap.hpp"
#include "Platform.hpp"

class Player : public Util::GameObject {
public:
    Player(std::shared_ptr<TileMap> tileMap);
    void Update();
    void SetTileMap(std::shared_ptr<TileMap> tileMap);

    glm::vec2 GetPosition() const;
    void SetRespawnPos(const glm::vec2& pos);
    void Respawn();
    void Die();
    bool IsDead() const;
    void SetPlatforms(const std::vector<std::shared_ptr<Platform>>& platforms);

private:
    bool IsOnSurface() const;
    // glm::vec2 GetHalfSize() const;
    bool CanMoveTo(const glm::vec2& position) const;
    glm::vec2 GetRidingPlatformDelta() const;

    void MoveX(float amount);
    void MoveY(float amount);

private:
    std::shared_ptr<TileMap> m_TileMap;

    std::shared_ptr<Util::Animation> m_RightDownAnimation;
    std::shared_ptr<Util::Animation> m_LeftDownAnimation;
    std::shared_ptr<Util::Animation> m_RightUpAnimation;
    std::shared_ptr<Util::Animation> m_LeftUpAnimation;

    std::shared_ptr<Util::Image> m_RightDownImage;
    std::shared_ptr<Util::Image> m_LeftDownImage;
    std::shared_ptr<Util::Image> m_RightUpImage;
    std::shared_ptr<Util::Image> m_LeftUpImage;

    std::shared_ptr<Util::Image> m_RightDownDeathImage;
    std::shared_ptr<Util::Image> m_LeftDownDeathImage;
    std::shared_ptr<Util::Image> m_RightUpDeathImage;
    std::shared_ptr<Util::Image> m_LeftUpDeathImage;

    glm::vec2 m_Velocity = {0.0f, 0.0f};
    float m_Gravity = 500.0f;
    bool m_GravityDown = false;
    bool m_FacingRight = true;

    bool m_IsDead = false;
    float m_DeathTimer = 0.0f;
    float m_DeathDuration = 1.0f;
    float m_BlinkInterval = 0.05f;
    glm::vec2 m_RespawnPos = {-200.0f, -150.0f};
    bool m_RespawnGravityDown = false;
    std::vector<std::shared_ptr<Platform>> m_Platforms;
};

#endif