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

std::vector<SingleBomb> Bombs::allBombs = {};


bool SingleBomb::operator==(const SingleBomb &other) const {
    return bombPos.x == other.bombPos.x && bombPos.y == other.bombPos.y;
}


void Bombs::checkExplosion() {
}

void Bombs::Update() {
    for (auto it = allBombs.begin(); it != allBombs.end(); ++it) {
        SingleBomb& b = *it;

        b.time += b.bombSpeed * GetFrameTime();
        if (b.time >= 1.0f) {
            b.time = 1.0f;
            b.bombPos = b.targetPos;

            checkSound(b);
            Explode(b);
            it = allBombs.erase(it);
            --it;
        } else {
            b.bombPos = Vector2Lerp(b.originPos, b.targetPos, b.time);
        }
    }

    if (!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)) return;

    Vector2 targetPos = GetScreenToWorld2D(GetMousePosition(), camera);
    if (targetPos.x < 0 || targetPos.x > G::MAP_WIDTH - 1 || targetPos.y < 0 || targetPos.y > G::MAP_HEIGHT - 1) return;

    const int cost = IsKeyPressed(KEY_ONE) ? 10000 : 100000;

    if (MAIN_PLAYER._money.moneyBalance - cost < 0) return;

    Pixel* startPixel = MAIN_PLAYER.GetNearestCityFromPixel(&G::territoryMap[targetPos.x][targetPos.y]);

    if (startPixel == nullptr) return;

    MAIN_PLAYER._money.spendMoney(cost);
    allBombs.push_back({
        .targetPos = targetPos,
        .originPos = startPixel->ToVector2(),
        .bombPos =  startPixel->ToVector2(),
        .time = 0,
        .bombSpeed = 1,
        .radius = IsKeyPressed(KEY_ONE) ? 50.f : 300.f,
    });
}

void Bombs::Render() {
    for (SingleBomb &b: allBombs) {
        DrawCircleV(b.bombPos, 10, RED);
    }
}


void Bombs::Explode(SingleBomb &b) {
    for (int y = -b.radius; y <= b.radius; y++) {
        for (int x = -b.radius; x <= b.radius; x++) {
            const int px = Clamp((int) b.targetPos.x + x, 0, G::MAP_WIDTH - 1);
            const int py = Clamp((int) b.targetPos.y + y, 0, G::MAP_HEIGHT - 1);

            const float distance = sqrtf((float) (x * x + y * y)) / b.radius;
            const float noise = GetRandomValue(50, 100) / 100.0f;

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
                    if (nukedPixel->playerId < 0) continue;
                    G::players[nukedPixel->playerId].LoseOwnershipOfPixel(nukedPixel, true);
                }
            }
        }
    }
    UpdateTexture(G::explosionTexture, G::explosionImage.data);
}

void Bombs::checkSound(SingleBomb &bomb) {
    // Satz des Pythagoras kein Problem

    int a = playerPos.y - bomb.targetPos.y;
    int b = playerPos.x - bomb.targetPos.x;
    int c = sqrt(a * a + b * b);

    if (bomb.radius == 50) {
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
