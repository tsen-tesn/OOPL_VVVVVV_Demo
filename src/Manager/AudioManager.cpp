#include "AudioManager.hpp"

AudioManager& AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager() {
    m_GameBgm = std::make_unique<Util::BGM>("Resources/Sound/background.wav");
    m_GameBgm->SetVolume(60);

    m_JumpUpSfx = std::make_unique<Util::SFX>("Resources/Sound/jump.wav");
    m_JumpDownSfx = std::make_unique<Util::SFX>("Resources/Sound/jump2.wav");
    m_HurtSfx = std::make_unique<Util::SFX>("Resources/Sound/hurt.wav");
    m_SaveSfx = std::make_unique<Util::SFX>("Resources/Sound/save.wav");
}

void AudioManager::PlayGameBgm() {
    if (m_IsGameBgmPlaying) {
        return;
    }

    m_GameBgm->Play();
    m_IsGameBgmPlaying = true;
}

void AudioManager::PlayJumpUp() {
    m_JumpUpSfx->Play();
}

void AudioManager::PlayJumpDown() {
    m_JumpDownSfx->Play();
}

void AudioManager::PlayHurt() {
    m_HurtSfx->Play();
}

void AudioManager::PlaySave() {
    m_SaveSfx->Play();
}
