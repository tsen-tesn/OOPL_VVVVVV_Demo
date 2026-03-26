#include "Platform.hpp"
#include "Util/Logger.hpp"

Platform::Platform() {
    LOG_INFO("Platform created");
}

void Platform::Update() {
    
    LOG_INFO("Platform updated");
}

bool Platform::IsSolid() const {
    return true;
}