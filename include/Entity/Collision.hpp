#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Util/GameObject.hpp"

namespace Collision {

inline constexpr float kPlayerHalfW       = 24.0f * 3.0f / 2.0f;
inline constexpr float kPlayerHalfH       = 24.0f * 3.0f / 2.0f;
inline constexpr float kPlayerShrinkLeft  = 21.0f;
inline constexpr float kPlayerShrinkRight = 20.0f;
inline constexpr float kPlayerShrinkTop   = 5.0f;
inline constexpr float kPlayerShrinkBot   = 9.0f;

struct Rect {
    float left;
    float right;
    float top;
    float bottom;
};

inline bool Overlaps(const Rect& a, const Rect& b) {
    return a.right > b.left &&
           a.left < b.right &&
           a.bottom > b.top &&
           a.top < b.bottom;
}

inline bool Touches(const Rect& a, const Rect& b) {
    return a.right >= b.left &&
           a.left <= b.right &&
           a.bottom >= b.top &&
           a.top <= b.bottom;
}

inline Rect Expanded(Rect rect, const glm::vec2& amount) {
    rect.left -= amount.x;
    rect.right += amount.x;
    rect.top -= amount.y;
    rect.bottom += amount.y;
    return rect;
}

inline Rect PlayerRect(const glm::vec2& position) {
    return {
        position.x - kPlayerHalfW + kPlayerShrinkLeft,
        position.x + kPlayerHalfW - kPlayerShrinkRight,
        position.y - kPlayerHalfH + kPlayerShrinkTop,
        position.y + kPlayerHalfH - kPlayerShrinkBot,
    };
}

inline Rect ObjectRect(const Util::GameObject& object, const glm::vec2& inset = {0.0f, 0.0f}) {
    const glm::vec2 pos  = object.GetTransform().translation;
    const glm::vec2 half = object.GetScaledSize() * 0.5f - inset;

    return {
        pos.x - half.x,
        pos.x + half.x,
        pos.y - half.y,
        pos.y + half.y,
    };
}

inline Rect CenterRect(const glm::vec2& position, const glm::vec2& halfSize) {
    return {
        position.x - halfSize.x,
        position.x + halfSize.x,
        position.y - halfSize.y,
        position.y + halfSize.y,
    };
}

} // namespace Collision

#endif
