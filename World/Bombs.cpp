//
// Created by minhp on 15.07.2025.
//

#include "Bombs.h"

#include <iostream>
#include <ostream>

#include "raylib.h"
#include "raymath.h"
#include "Globals.h"
#include "Loaders/Sounds.h"

#define MAIN_PLAYER G::players[0]

extern Camera2D camera;
extern Vector2 playerPos;
extern Sounds mySounds;

Vector2 Bombs::originPos = {};
Vector2 Bombs::targetPos = {};
Bombs activeBomb;

void Bombs::checkExplosion() {
    if (!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)) return;

    const int cost = IsKeyPressed(KEY_ONE) ? 10000 : 100000;

    if (MAIN_PLAYER._money.moneyBalance - cost < 0) return;
    MAIN_PLAYER._money.spendMoney(cost);

    targetPos = GetScreenToWorld2D(GetMousePosition(), camera);
    const int radius = IsKeyPressed(KEY_ONE) ? 50 : 300;
    originPos = {0, 0};

    // Initializing
    activeBomb = Bombs{};
    activeBomb.time = 0.0f;
    activeBomb.radius = radius;
    activeBomb.bombSpeed = 0.5f;
    activeBomb.isActive = true;
    activeBomb.isExploding = false;
    activeBomb.bombPos = originPos;
}

void Bombs::Update()
{
    if (!isActive || isExploding) return;

    time += bombSpeed * GetFrameTime();
    if (time >= 1.0f) {
        time = 1.0f;
        bombPos = targetPos;
        isActive = false;
        isExploding = true;

        checkSound(radius);
        Explode(radius);
        return;
    }

    bombPos = Vector2Lerp(originPos, targetPos, time);

    DrawCircleV(bombPos, 10, RED);
}

void Bombs::Explode(const int radius)
{
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int px = (int) targetPos.x + x;
            int py = (int) targetPos.y + y;

            float distance = sqrtf((float) (x * x + y * y)) / radius;
            float noise = GetRandomValue(50, 100) / 100.0f;

            if (distance <= 1.0f && noise > distance) {
                if (px >= 0 && py >= 0 && px < G::MAP_WIDTH && py < G::MAP_HEIGHT) {
                    Color explosionColor = Color{
                        (unsigned char) GetRandomValue(0, 200),
                        (unsigned char) GetRandomValue(230, 255),
                        (unsigned char) GetRandomValue(0, 50),
                        255
                    };

                    ImageDrawPixel(&G::explosionImage, px, py, explosionColor);

                    Pixel *nukedPixel = &G::territoryMap[px][py];
                    for (Player &p: G::players) {
                        if (p._allPixels.contains(nukedPixel)) {
                            p.LoseOwnershipOfPixel(nukedPixel, true);
                        }
                    }
                }
            }
        }
    }
    UpdateTexture(G::explosionTexture, G::explosionImage.data);
}

void Bombs::checkSound(const int radiusComparison)
{
    // Satz des Pythagoras kein Problem

    int a = playerPos.y - targetPos.y;
    int b = playerPos.x - targetPos.x;
    int c = sqrt(a * a + b * b);

    if (radiusComparison == 50) {
        if (c >= 1000) {
            mySounds.Play(mySounds.distantExplosionSound);
        } else if (c >= 500) {
            mySounds.Play(mySounds.farExplosionSound);
        } else {
            mySounds.Play(mySounds.explosionSound);
        }
    } else {
        if (c >= 1000) {
            mySounds.Play(mySounds.farExplosionSound);
        } else {
            mySounds.Play(mySounds.explosionSound);
        }
    }
}