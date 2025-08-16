//
// Created by yanam on 02.07.2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <climits>
#include <vector>
#include <cmath>
#include "raylib.h"
#include "player/Player.h"
#include "map/ChunkGeneration.h"
#include "loaders/Sounds.h"

struct Gradient;


// G for global
namespace G {
    constexpr int MAP_WIDTH = 4 * 1028; // 2500
    constexpr int MAP_HEIGHT = 4 * 1028; // 2500
    constexpr int SCREEN_WIDTH = 1366;
    constexpr int SCREEN_HEIGHT = 768;

    inline Camera2D camera{};

    inline bool showLeaderboard = true;
    inline bool showControls = true;

    inline Sounds mySounds;

    inline float maxDifficulty = 30;

    inline std::vector<Player> players;
    inline Vector2 playerPos{MAP_WIDTH / 2, MAP_HEIGHT / 2};

    inline bool gameOver{};
    inline int winnerId = -1;

    inline Image explosionImage;
    inline Texture2D explosionTexture;
    inline bool explosionTextureDirty{};

    inline std::vector<Pixel> territoryMap; // [x * MAP_HEIGHT + y]
    inline Texture2D territoryTexture;
    inline Image territoryImage;
    inline bool territoryTextureDirty{};

    inline int ToIdx(const int x, const int y) {
        return x * MAP_HEIGHT + y;
    }

    inline Pixel *PixelAt(const int x, const int y) {
        if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT) return nullptr;
        return &territoryMap[ToIdx(x, y)];
    }

    inline Pixel *PixelAt(const Vector2 v) {
        return PixelAt(
            static_cast<int>(v.x),
            static_cast<int>(v.y)
        );
    }

    inline Pixel *GetPixelOnMouse() {
        return PixelAt(
            static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).x),
            static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).y)
        );
    }

    inline Vector2 GetChunkFromV(Vector2 pos) {
        auto chunkSize = static_cast<float>(ChunkGeneration::chunkSize);
        float cx = floor(pos.x / chunkSize);
        float cy = floor(pos.y / chunkSize);
        return { cx, cy };
    }

    // randomness
    static unsigned long x = 123456789, y = 362436069, z = 521288629;

    inline unsigned long xorshf96() {
        unsigned long t;
        x ^= x << 16;
        x ^= x >> 5;
        x ^= x << 1;

        t = x;
        x = y;
        y = z;
        z = t ^ x ^ y;

        return z;
    }

    inline float RandomFloat_0to1() {
        return static_cast<float>(xorshf96()) / static_cast<float>(ULONG_MAX);
    }

    inline Pixel *GetRandPixel() {
        const int randIdx = xorshf96() % (MAP_WIDTH * MAP_HEIGHT);
        return &territoryMap[randIdx];
    }
}

#endif //GLOBALS_H
