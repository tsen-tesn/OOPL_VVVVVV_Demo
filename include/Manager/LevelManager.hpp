#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <glm/vec2.hpp>

#include "json.hpp"
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
    const nlohmann::json& GetRoomJson(int roomID);
    void PreloadRoom(int roomID);
    void PreloadConnectedRooms();
    bool TryLoadConnectedRoom(
        int roomID,
        glm::vec2& playerPosition,
        const glm::vec2& wrappedPosition,
        const glm::vec2& blockedPosition);

private:
    std::shared_ptr<LoadLevel> m_CurrentLevel;
    std::unordered_map<int, nlohmann::json> m_RoomJsonCache;
    std::unordered_set<int> m_PreloadRooms;
    int m_CurrentRoomID = 1;
    int m_RespawnRoomID = 1;

    static constexpr float kHalfW = 520.0f;
    static constexpr float kHalfH = 330.0f;
    static constexpr float kWrapOffset = 10.0f;
};

#endif // LEVELMANAGER_HPP
