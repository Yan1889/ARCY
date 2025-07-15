//
// Created by minhp on 15.07.2025.
//

#ifndef BOMBS_H
#define BOMBS_H

#include "raylib.h"


class Bombs {
    public:
    static Vector2 targetPos;
    static Vector2 originPos;
    float bombSpeed = 0.2f;
    float time = 0.0f;
    float radius = 0.0f;
    bool isActive = false;
    bool isExploding = false;
    Vector2 bombPos;

    void checkExplosion();
    void Explode(int radius);
    void checkSound(int radiusComparison);
    void Update();
};

extern Bombs activeBomb;



#endif //BOMBS_H
