#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <memory>
#include <string>

#include <glm/vec2.hpp>

#include "Loadlevel.hpp"

class LevelManager {
public:
    explicit LevelManager(int startRoomID = 1);

    void LoadRoom(int roomID);
    bool TryTransition(glm::vec2& playerPosition);

    const std::shared_ptr<LoadLevel>& GetCurrentLevel() const { return m_CurrentLevel; }
    int GetCurrentRoomID() const { return m_CurrentRoomID; }

    void SetRespawnRoomID(int roomID) { m_RespawnRoomID = roomID; }
    int GetRespawnRoomID() const { return m_RespawnRoomID; }

private:
    static std::string BuildRoomPath(int roomID);
    bool TryLoadConnectedRoom(
        int roomID,
        glm::vec2& playerPosition,
        const glm::vec2& wrappedPosition,
        const glm::vec2& blockedPosition);

private:
    std::shared_ptr<LoadLevel> m_CurrentLevel;
    int m_CurrentRoomID = 1;
    int m_RespawnRoomID = 1;

    static constexpr float kHalfW = 520.0f;
    static constexpr float kHalfH = 330.0f;
    static constexpr float kWrapOffset = 10.0f;
};

#endif // LEVELMANAGER_HPP
