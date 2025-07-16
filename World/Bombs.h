//
// Created by minhp on 15.07.2025.
//

#ifndef BOMBS_H
#define BOMBS_H

#include <vector>

#include "raylib.h"

struct SingleBomb {
    Vector2 targetPos;
    Vector2 originPos;
    Vector2 bombPos;
    float time;
    float bombSpeed;
    float radius;

    bool operator==(const SingleBomb& other) const;
};

class Bombs {
    static std::vector<SingleBomb> allBombs;
    static void checkExplosion();
    static void Explode(SingleBomb& bomb);
    static void checkSound(SingleBomb& bomb);

public:
    static void Update();
    static void Render();
};

extern Bombs activeBomb;



#endif //BOMBS_H
