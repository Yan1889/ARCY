//
// Created by minhp on 17.07.2025.
//

#ifndef SOUNDPOOL_H
#define SOUNDPOOL_H

#include "raylib.h"
#include <string>
#include <vector>

class SoundPool {
public:
    SoundPool(const std::string& soundFilePath, int poolSize = 8);
    ~SoundPool();

    void Play();
    void SetVolume(float volume);
    void StopAll();

private:
    std::vector<Sound> pool;
    int currentIndex = 0;
};

#endif //SOUNDPOOL_H
