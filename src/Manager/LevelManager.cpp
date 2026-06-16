#include "Manager/LevelManager.hpp"

#include "Util/Logger.hpp"

#include <fstream>

LevelManager::LevelManager(int startRoomID) {
    LoadRoom(startRoomID);
    m_RespawnRoomID = startRoomID;
}

void LevelManager::LoadRoom(int roomID) {
    m_CurrentLevel = std::make_shared<LoadLevel>(GetRoomJson(roomID));
    m_PreloadRooms.insert(roomID);
    m_CurrentRoomID = roomID;
    PreloadConnectedRooms();
}

bool LevelManager::TryTransition(glm::vec2& playerPosition) {
    const auto connections = m_CurrentLevel->GetConnections();

    if (playerPosition.x >= kHalfW) {
        if (connections.right == -1) {
            playerPosition.x = kHalfW;
            return false;
        }
        return TryLoadConnectedRoom(
            connections.right,
            playerPosition,
            {-kHalfW + kWrapOffset, playerPosition.y},
            {kHalfW, playerPosition.y});
    }

    if (playerPosition.x <= -kHalfW) {
        if (connections.left == -1) {
            playerPosition.x = -kHalfW;
            return false;
        }
        return TryLoadConnectedRoom(
            connections.left,
            playerPosition,
            {kHalfW - kWrapOffset, playerPosition.y},
            {-kHalfW, playerPosition.y});
    }

    if (playerPosition.y >= kHalfH) {
        if (connections.up == -1) {
            playerPosition.y = kHalfH;
            return false;
        }
        return TryLoadConnectedRoom(
            connections.up,
            playerPosition,
            {playerPosition.x, -kHalfH + kWrapOffset},
            {playerPosition.x, kHalfH});
    }

    if (playerPosition.y <= -kHalfH) {
        if (connections.down == -1) {
            playerPosition.y = -kHalfH;
            return false;
        }
        return TryLoadConnectedRoom(
            connections.down,
            playerPosition,
            {playerPosition.x, kHalfH - kWrapOffset},
            {playerPosition.x, -kHalfH});
    }

    return false;
}

std::string LevelManager::BuildRoomPath(int roomID) {
    return std::string(RESOURCE_DIR) + "/Map/VVVVVV Demo/room" + std::to_string(roomID) + ".json";
}

const nlohmann::json& LevelManager::GetRoomJson(int roomID) {
    const auto cachedJson = m_RoomJsonCache.find(roomID);
    if (cachedJson != m_RoomJsonCache.end()) {
        return cachedJson->second;
    }

    const std::string roomPath = BuildRoomPath(roomID);
    std::ifstream file(roomPath);
    if (!file.is_open()) {
        throw std::runtime_error("LevelManager: cannot open " + roomPath);
    }

    nlohmann::json levelJson;
    try {
        file >> levelJson;
    } catch (nlohmann::json::parse_error& e) {
        throw std::runtime_error("LevelManager JSON parse error: " + std::string(e.what()));
    }

    const auto result = m_RoomJsonCache.emplace(roomID, std::move(levelJson));
    return result.first->second;
}

void LevelManager::PreloadRoom(int roomID) {
    if (roomID == -1 || m_PreloadRooms.find(roomID) != m_PreloadRooms.end()) {
        return;
    }

    try {
        LoadLevel(GetRoomJson(roomID));
        m_PreloadRooms.insert(roomID);
    } catch (const std::exception& e) {
        LOG_WARN("Failed to preload room {}: {}", roomID, e.what());
    }
}

void LevelManager::PreloadConnectedRooms() {
    if (!m_CurrentLevel) {
        return;
    }

    const auto connections = m_CurrentLevel->GetConnections();
    PreloadRoom(connections.up);
    PreloadRoom(connections.right);
    PreloadRoom(connections.down);
    PreloadRoom(connections.left);
}

bool LevelManager::TryLoadConnectedRoom(
    int roomID,
    glm::vec2& playerPosition,
    const glm::vec2& wrappedPosition,
    const glm::vec2& blockedPosition) {
    try {
        LoadRoom(roomID);
        playerPosition = wrappedPosition;
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load room {}: {}", roomID, e.what());
        playerPosition = blockedPosition;
        return false;
    }
}
