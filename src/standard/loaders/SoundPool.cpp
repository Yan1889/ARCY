//
// Created by minhp on 17.07.2025.
//

#include "SoundPool.h"

SoundPool::SoundPool(const std::string& soundFilePath, int poolSize)
{
    pool.reserve(poolSize);
    for (int i = 0; i < poolSize; ++i) {
        Sound s = LoadSound(soundFilePath.c_str());
        pool.push_back(s);
    }
}

SoundPool::~SoundPool()
{
    for (auto& s : pool) {
        UnloadSound(s);
    }
}

void SoundPool::Play()
{
    PlaySound(pool[currentIndex]);
    currentIndex = (currentIndex + 1) % pool.size();
}

void SoundPool::SetVolume(float volume)
{
    for (auto& s : pool) {
        SetSoundVolume(s, volume);
    }
}

void SoundPool::StopAll()
{
    for (auto& s : pool) {
        StopSound(s);
    }
}