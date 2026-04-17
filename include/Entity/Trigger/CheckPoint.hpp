#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

#include <string>
#include "Util/Image.hpp"
#include "Entity/Trigger/Trigger.hpp"

class CheckPoint : public Trigger {
public:
    CheckPoint(const glm::vec2& position, const std::string& imagePath);
    bool is_touched(const glm::vec2 &playerPos) const override;
    void SetActivated(bool activated);
    bool IsActivated() const { return m_isActivated; }

private:
    bool m_isActivated = false;
};

#endif
