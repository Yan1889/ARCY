//
// Created by minhp on 15.07.2025.
//

#include "Bombs.h"

#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "Globals.h"
#include "loaders/TextureCollection.h"
#include "map/CameraClipping.h"

#define MAIN_PLAYER players[0]

using namespace CameraClipping;

std::vector<SingleBomb> Bombs::allBombs = {};
std::vector<RadiationZone> Bombs::allZones = {};
std::vector<VisualEffectAfterDetonation> Bombs::allEffects = {};

int Bombs::atomBombCost = 100'000;
int Bombs::hydrogenBombCost = 1'000'000;

bool SingleBomb::operator==(const SingleBomb &other) const {
    return pos.x == other.pos.x && pos.y == other.pos.y;
}

void Bombs::Update() {
    for (auto it = allBombs.begin(); it != allBombs.end(); ++it) {
        SingleBomb& b = *it;

        float distance = Vector2Distance(b.originPos, b.targetPos);
        float duration = distance / b.speed;

        b.time += b.speed * GetFrameTime();

        if (b.time >= duration) {
            b.time = duration;
            b.pos = b.targetPos;

            checkSound(b);
            Explode(b);
            allEffects.push_back({
                .pos = b.pos,
                .radius = b.radius,
            });
            allZones.push_back(RadiationZone{
                .centerPos = PixelAt(b.pos),
                .radius = (int) b.radius,
            });
            it = allBombs.erase(it);
            --it;
        } else {
            float t = b.time / duration;

            b.pos = Vector2Lerp(b.originPos, b.targetPos, t);
        }
    }
    for (auto it = allZones.begin(); it != allZones.end(); ++it) {
        RadiationZone& z = *it;

        z.timeLeft -= GetFrameTime();

        if (z.timeLeft < 0.0f) {
            // remove radiation
            for (int y = -z.radius; y <= z.radius; y++) {
                for (int x = -z.radius; x <= z.radius; x++) {
                    const int px = Clamp(z.centerPos->x + x, 0, MAP_WIDTH - 1);
                    const int py = Clamp(z.centerPos->y + y, 0, MAP_HEIGHT - 1);

                    const float distance = sqrtf((float) (x * x + y * y)) / z.radius;

                    if (distance <= 1) {
                        Pixel *p = PixelAt(px, py);
                        if (p->playerId == -2) {
                            ImageDrawPixel(&explosionImage, px, py, BLANK);
                            p->playerId = -1;
                            p->contaminated = false;
                        }
                    }
                }
            }
            it = allZones.erase(it);
            --it;

            explosionTextureDirty = true;
        }
    }
}

void Bombs::RenderBomb() {
    for (SingleBomb &b: allBombs) {
        Texture2D& t = b.type == ATOM? TextureCollection::atomBomb : TextureCollection::hydrogenBomb;
        const float scale = b.type == ATOM? 0.2 : 0.5;

        if (CheckCollisionCameraCircle(b.pos, t.height / 2, GetViewRectangle(camera))) continue;

        const float dx = b.targetPos.x - b.pos.x;
        const float dy = b.targetPos.y - b.pos.y;
        const float rotation = std::atan2(dy, dx);

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
}

void Bombs::RenderFlash()
{
    for (auto it = allEffects.begin(); it != allEffects.end(); ++it) {
        VisualEffectAfterDetonation& e = *it;
        e.timeLeft -= GetFrameTime();

        if (e.timeLeft < 0) {
            it = allEffects.erase(it);
            --it;
        } else {
            e.rotation += e.rotationStep * GetFrameTime();

            Texture2D* t = &TextureCollection::explosion;
            float scale = 2 * e.radius / t->width;
            if (!CheckCollisionCameraCircle(e.pos, t->width * scale / 2, GetViewRectangle(camera)))
            {
                std::cout << "Drawing explosion" << std::endl;
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
            }

            t = &TextureCollection::flash;
            scale = 1.5 * 2 * e.radius / t->width;
            if (!CheckCollisionCameraCircle(e.pos, t->width * scale / 2, GetViewRectangle(camera)))
            {
                std::cout << "Drawing flash" << std::endl;
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
