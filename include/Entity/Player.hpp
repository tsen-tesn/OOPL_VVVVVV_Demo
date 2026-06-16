#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Core/Drawable.hpp"
#include "Collision.hpp"
#include "TileMap.hpp"
#include "Platform.hpp"

class Player : public Util::GameObject {
public:
    explicit Player(std::shared_ptr<TileMap> tileMap);

    // === 每幀更新 ===
    void Update();

    // === 狀態查詢 ===
    glm::vec2 GetPosition() const;
    bool IsDead() const;

    // === 外部設定 ===
    void SetTileMap(std::shared_ptr<TileMap> tileMap);
    void SetPlatforms(const std::vector<std::shared_ptr<Platform>>& platforms);
    void SetRespawnPos(const glm::vec2& pos);

    // === 死亡 / 復活 ===
    void Die();
    void Respawn();

private:
    // ─── 碰撞常數（集中定義，三個碰撞方法共用）───────────────────────
    static constexpr float kHalfW       = Collision::kPlayerHalfW; // 36.0f
    static constexpr float kHalfH       = Collision::kPlayerHalfH; // 36.0f
    static constexpr float kShrinkLeft  = Collision::kPlayerShrinkLeft;
    static constexpr float kShrinkRight = Collision::kPlayerShrinkRight;
    static constexpr float kShrinkTop   = Collision::kPlayerShrinkTop;
    static constexpr float kShrinkBot   = Collision::kPlayerShrinkBot;
    static constexpr float kProbeOffset = 10.0f;

    // ─── Update 子步驟 ────────────────────────────────────────────────
    void HandleDeathAnimation(float dt);  ///< 死亡閃爍 + 計時自動復活
    void HandleInput();                   ///< 左右移動 + 貼地重力翻轉
    void ApplyPhysics(float dt);          ///< 套用速度 → 分軸位移
    void ApplyPlatformRiding();           ///< 移動平台跟隨
    void UpdateAnimation();               ///< 依 facing / gravity / moving 選圖

    // ─── 碰撞查詢（供 Apply* 呼叫）──────────────────────────────────
    bool CanMoveTo(const glm::vec2& pos) const;
    bool IsOnSurface() const;
    glm::vec2 GetRidingPlatformDelta() const;

    // ─── 動畫輔助 ────────────────────────────────────────────────────
    /// 依 facing / gravity 選出正確的走路動畫或 idle 圖
    std::shared_ptr<Core::Drawable> SelectAnimation(bool isMoving) const;
    /// 依 facing / gravity 選出死亡閃爍圖
    std::shared_ptr<Core::Drawable> SelectDeathImage() const;

private:
    std::shared_ptr<TileMap> m_TileMap;

    // 走路動畫（right+down, left+down, right+up, left+up）
    std::shared_ptr<Util::Animation> m_Animations[4];
    // Idle 靜止圖
    std::shared_ptr<Util::Image>     m_IdleImages[4];
    // 死亡閃爍圖
    std::shared_ptr<Util::Image>     m_DeathImages[4];

    glm::vec2 m_Velocity        = {0.0f, 0.0f};
    float     m_Gravity         = 500.0f;
    bool      m_GravityDown     = false;
    bool      m_FacingRight     = true;

    bool      m_IsDead          = false;
    float     m_DeathTimer      = 0.0f;
    float     m_DeathDuration   = 1.0f;
    float     m_BlinkInterval   = 0.05f;
    glm::vec2 m_RespawnPos      = {-200.0f, -150.0f};
    bool      m_RespawnGravityDown = false;

    std::vector<std::shared_ptr<Platform>> m_Platforms;
};

#endif
