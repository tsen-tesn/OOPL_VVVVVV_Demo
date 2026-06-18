#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <memory>

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"

class AudioManager {
public:
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    static AudioManager& GetInstance();

    void PlayGameBgm();
    void PauseGameBgm();
    void PlayJumpUp();
    void PlayJumpDown();
    void PlayHurt();
    void PlaySave();
    void PlayGameComplete();

private:
    AudioManager();

private:
    std::unique_ptr<Util::BGM> m_GameBgm;
    std::unique_ptr<Util::SFX> m_JumpUpSfx;
    std::unique_ptr<Util::SFX> m_JumpDownSfx;
    std::unique_ptr<Util::SFX> m_HurtSfx;
    std::unique_ptr<Util::SFX> m_SaveSfx;
    std::unique_ptr<Util::SFX> m_GameCompleteSfx;

    bool m_IsGameBgmPlaying = false;
};

#endif // AUDIOMANAGER_HPP
