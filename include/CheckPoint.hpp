#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

#include "Trigger.hpp"

class CheckPoint : public Trigger {
public:
    CheckPoint(const glm::vec2& position, const std::string& imagePath);
    bool is_touched(const glm::vec2 &playerPos) const override;
    void Update() override;
private:
    std::shared_ptr<Util::Image> m_Image;
};

#endif
