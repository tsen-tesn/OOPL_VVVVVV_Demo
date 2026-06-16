#include "Manager/LevelManager.hpp"

#include "Util/Logger.hpp"

LevelManager::LevelManager(int startRoomID) {
    LoadRoom(startRoomID);
    m_RespawnRoomID = startRoomID;
}

void LevelManager::LoadRoom(int roomID) {
    m_CurrentLevel = std::make_shared<LoadLevel>(BuildRoomPath(roomID));
    m_CurrentRoomID = roomID;
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
