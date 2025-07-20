//
// Created by minhp on 15.07.2025.
//

#include "Bombs.h"

#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "Globals.h"
#include "loaders/TextureCollection.h"

#define MAIN_PLAYER players[0]


std::vector<SingleBomb> Bombs::allBombs = {};
std::vector<EffectAfterDetonation> Bombs::allEffects = {};

bool SingleBomb::operator==(const SingleBomb &other) const {
    return pos.x == other.pos.x && pos.y == other.pos.y;
}

void Bombs::Update() {
    for (auto it = allBombs.begin(); it != allBombs.end(); ++it) {
        SingleBomb& b = *it;

        b.time += b.speed * GetFrameTime();

        if (b.time >= 1.0f) {
            b.time = 1.0f;
            b.pos = b.targetPos;

            checkSound(b);
            Explode(b);
            allEffects.push_back({
                .pos = b.pos,
                .radius = b.radius,
            });
            it = allBombs.erase(it);
            --it;
        } else {
            b.pos = Vector2Lerp(b.originPos, b.targetPos, b.time);
        }
    }

    if (!IsKeyDown(KEY_ONE) && !IsKeyDown(KEY_TWO)) return;

    Vector2 targetPos = GetScreenToWorld2D(GetMousePosition(), camera);
    if (targetPos.x < 0 || targetPos.x > MAP_WIDTH - 1 || targetPos.y < 0 || targetPos.y > MAP_HEIGHT - 1) return;

    const int cost = IsKeyDown(KEY_ONE) ? 10000 : 100000;

    if (MAIN_PLAYER._money.moneyBalance - cost < 0) return;

    Pixel* startPixel = MAIN_PLAYER.GetNearestSiloFromPixel(PixelAt(targetPos));

    if (startPixel == nullptr) return;

    MAIN_PLAYER._money.spendMoney(cost);
    
    mySounds.Play(mySounds.misslePool);

    allBombs.push_back({
        .targetPos = targetPos,
        .originPos = startPixel->ToVector2(),
        .pos =  startPixel->ToVector2(),
        .time = 0,
        .speed = 1,
        .radius = IsKeyDown(KEY_ONE) ? 50.f : 300.f,
        .type = IsKeyDown(KEY_ONE)? ATOM : HYDROGEN
    });
}

void Bombs::Render() {
    for (SingleBomb &b: allBombs) {
        const float dx = b.targetPos.x - b.pos.x;
        const float dy = b.targetPos.y - b.pos.y;
        const float rotation = std::atan2(dy, dx);

        Texture2D& t = b.type == ATOM? TextureCollection::atomBomb : TextureCollection::hydrogenBomb;
        const float scale = b.type == ATOM? 0.2 : 0.5;

        DrawTexturePro(
            t,
            Rectangle{0, 0, (float) t.width, (float) t.height},
            Rectangle{
                b.pos.x,
                b.pos.y,
                t.width * scale,
                t.height * scale,
            },
            Vector2{
                t.width / 2.f * scale,
                t.height / 2.f * scale,
            },
            rotation * 180 / PI - 90,
            WHITE
        );
    }
    for (auto it = allEffects.begin(); it != allEffects.end(); ++it) {
        EffectAfterDetonation& e = *it;
        e.timeLeft -= GetFrameTime();

        if (e.timeLeft < 0) {
            it = allEffects.erase(it);
            --it;
        } else {
            e.rotation += e.rotationStep * GetFrameTime();

            Texture2D* t = &TextureCollection::explosion;
            float scale = 2 * e.radius / t->width;
            DrawTexturePro(
                *t,
                Rectangle{0, 0, (float) t->width, (float) t->height},
                Rectangle{
                    e.pos.x,
                    e.pos.y,
                    t->width * scale,
                    t->height * scale,
                },
                Vector2{
                    t->width / 2.f * scale,
                    t->height / 2.f * scale,
                },
                e.rotation,
                WHITE
            );

            t = &TextureCollection::flash;
            scale = 1.5 * 2 * e.radius / t->width;
            DrawTexturePro(
                *t,
                Rectangle{0, 0, (float) t->width, (float) t->height},
                Rectangle{
                    e.pos.x,
                    e.pos.y,
                    t->width * scale,
                    t->height * scale,
                },
                Vector2{
                    t->width / 2.f * scale,
                    t->height / 2.f * scale,
                },
                -2 * e.rotation + 90,
                WHITE
            );
        }
    }
}


void Bombs::Explode(SingleBomb &b) {
    for (int y = -b.radius; y <= b.radius; y++) {
        for (int x = -b.radius; x <= b.radius; x++) {
            const int px = Clamp((int) b.targetPos.x + x, 0, MAP_WIDTH - 1);
            const int py = Clamp((int) b.targetPos.y + y, 0, MAP_HEIGHT - 1);

            const float distance = sqrtf((float) (x * x + y * y)) / b.radius;
            const float noise = GetRandomValue(50, 100) / 100.0f;

            if (distance <= 1.0f && noise > distance) {
                if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {
                    const Color explosionColor{
                        (unsigned char) GetRandomValue(0, 200),
                        (unsigned char) GetRandomValue(230, 255),
                        (unsigned char) GetRandomValue(0, 50),
                        255
                    };

                    ImageDrawPixel(&explosionImage, px, py, explosionColor);

                    Pixel *nukedPixel = PixelAt(px, py);
                    nukedPixel->contaminated = true;

                    // remove from player
                    if (nukedPixel->playerId >= 0) {
                        players[nukedPixel->playerId].LoseOwnershipOfPixel(nukedPixel, true);
                    }
                    nukedPixel->playerId = -2;
                }
            }
        }
    }
    explosionTextureDirty = true;
}

void Bombs::checkSound(SingleBomb &bomb) {
    // Satz des Pythagoras kein Problem

    int a = playerPos.y - bomb.targetPos.y;
    int b = playerPos.x - bomb.targetPos.x;
    int c = sqrt(a * a + b * b);

    if (bomb.radius == 50) {
        if (c >= 1000) {
            mySounds.Play(mySounds.distantExplosionPool);
        } else if (c >= 500) {
            mySounds.Play(mySounds.nearExplosionPool);
        } else {
            mySounds.Play(mySounds.explosionPool);
        }
    } else {
        if (c >= 1000) {
            mySounds.Play(mySounds.nearExplosionPool);
        } else {
            mySounds.Play(mySounds.explosionPool);
        }
    }
}
