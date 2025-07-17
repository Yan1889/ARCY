#include "raylib.h"
#include "SoundPool.h"
#include <vector>

#ifndef SOUNDS_H
#define SOUNDS_H

extern Vector2 playerPos;

class Sounds {

public:
    Sounds();
    ~Sounds();

    void LoadAllExplosion();
    void PlayExplosion();

    Sound explosionSound;
    Sound farExplosionSound;
    Sound distantExplosionSound;
    Sound cityBuildSound;
    Sound missleSound;

    static std::vector<Sound> explosionSounds;
    static int currentIndex;

    Music oceanSound;
    Music beachSound;
    Music fieldSound;
    Music forestSound;
    Music mountainSound;
    Music radiationSound;

    static void Play(Sound sound);
    static void Stop(Sound sound);
    void LoadAll();
    void checkAtmosphere();

private:
    SoundPool* explosionPool = nullptr;
};

#endif //SOUNDS_H
