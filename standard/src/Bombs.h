//
// Created by minhp on 15.07.2025.
//

#ifndef BOMBS_H
#define BOMBS_H

#include <vector>

#include "raylib.h"
#include "Globals.h"

using namespace G;


struct SingleBomb {
    Vector2 targetPos{};
    Vector2 originPos{};
    Vector2 pos{};
    int radius{};
    bool isAtom;
    int speed{};

    float time{};
    bool operator==(const SingleBomb& other) const;
};

struct RadiationZone {
    Pixel *centerPos;
    int radius;
    float timeLeft = 10; // 20s
};

struct VisualEffectAfterDetonation {
    Vector2 pos;
    float radius;
    float timeLeft = 0.5;
    float rotation = 0;
    float rotationStep = 90; // 90°/s explosion; 180°/s flash
};

class Bombs {
    static std::vector<VisualEffectAfterDetonation> allEffects;
    static std::vector<RadiationZone> allZones;
    static void Explode(SingleBomb& bomb);
    static void checkSound(SingleBomb& bomb);

public:
    static std::vector<SingleBomb> allBombs;
    static void Update();
    static void RenderBomb();
    static void RenderFlash();

    static constexpr int ATOM_COST = 100'000;
    static constexpr int H_COST = 1'000'000;
    static constexpr int ATOM_RADIUS = 50;
    static constexpr int H_RADIUS = 350;
    static constexpr int ATOM_SPEED = 15;
    static constexpr int H_SPEED = 10;
};



#endif //BOMBS_H
