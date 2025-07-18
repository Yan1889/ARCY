#ifndef SOUNDS_H
#define SOUNDS_H

#include "raylib.h"
#include "SoundPool.h"
#include <vector>

class Sounds {

public:
    Sounds();
    ~Sounds();

    void LoadAll();
    void Play(SoundPool* pool);

    static std::vector<Sound> explosionSounds;
    static int currentIndex;

    Music oceanSound;
    Music beachSound;
    Music fieldSound;
    Music forestSound;
    Music mountainSound;
    Music radiationSound;

    void checkAtmosphere();

    SoundPool* explosionPool = nullptr;
    SoundPool* nearExplosionPool = nullptr;
    SoundPool* distantExplosionPool = nullptr;
    SoundPool* cityBuildPool = nullptr;
    SoundPool* misslePool = nullptr;
};

#endif //SOUNDS_H
