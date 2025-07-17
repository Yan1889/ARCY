#include "raylib.h"

#ifndef SOUNDS_H
#define SOUNDS_H

extern Vector2 playerPos;

class Sounds {

public:
    Sound explosionSound;
    Sound farExplosionSound;
    Sound distantExplosionSound;
    Sound cityBuildSound;
    Sound missleSound;

    Music oceanSound;
    Music beachSound;
    Music fieldSound;
    Music forestSound;
    Music mountainSound;
    Music radiationSound;

    Sounds();
    ~Sounds();
    static void Play(Sound sound);
    static void Stop(Sound sound);
    void LoadAll();
    void checkAtmosphere();
};

#endif //SOUNDS_H
