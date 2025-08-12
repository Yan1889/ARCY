//
// Created by yanam on 02.07.2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <iostream>
#include <vector>
#include "map/PerlinNoise.h"
#include "map/Pixel.h"
#include "raylib.h"
#include "player/Player.h"
#include "loaders/Sounds.h"

struct Gradient;


// G for global
namespace G {
    constexpr int MAP_WIDTH = 2500;
    constexpr int MAP_HEIGHT = 2500;
    constexpr int SCREEN_WIDTH = 1366;
    constexpr int SCREEN_HEIGHT = 768;

    inline int targetFPS = 60;

    inline Camera2D camera{};
    inline Image perlin;
    inline Texture2D perlinTexture{};
    inline std::vector<Gradient> mapParts = {
        Gradient{32 * 1 + 10, {90, 90, 255, 255}, 0.f}, // Deep Water
        Gradient{32 * 2 + 10, {125, 125, 255, 255}, 0.f}, // Low Water
        Gradient{32 * 2 + 20, {247, 252, 204, 255}, 0.5f}, // Beach
        Gradient{32 * 3 + 20, {129, 245, 109, 255}, 0.9f}, // Open Field
        Gradient{32 * 4 + 10, {117, 219, 99, 255}, 0.7f}, // Hills
        Gradient{32 * 5 + 20, {97, 184, 81, 255}, 0.3f}, // Forest
        Gradient{32 * 6, {191, 191, 191, 255}, 0.1f}, // Stone
        Gradient{32 * 7 - 20, {153, 153, 153, 255}, 0.f}, // Mountain
        Gradient{32 * 8, {255, 255, 255, 255}, 0.f}, // Snow
    };

    inline Sounds mySounds;

    inline float maxDifficulty = 30;

    inline std::vector<Player> players;
    inline Vector2 playerPos;

    inline bool gameOver{};
    inline int winnerId = -1;

    inline Image explosionImage;
    inline Texture2D explosionTexture;
    inline bool explosionTextureDirty{};
    inline void RemoveExplosionPixel(Pixel* p) {
        p->contaminated = false;
        ImageDrawPixel(&explosionImage, p->x, p->y, BLANK);
        explosionTextureDirty = true;
    }

    inline std::vector<std::vector<Pixel> > territoryMap; // [x][y]
    inline Texture2D territoryTexture;
    inline Image territoryImage;
    inline bool territoryTextureDirty{};

    inline Pixel* PixelAt(const int x, const int y) {

        if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT) return nullptr;

        return &territoryMap[x][y];
    }
    inline Pixel* PixelAt(Vector2 v) {
        int x = static_cast<int>(v.x);
        int y = static_cast<int>(v.y);

        if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT) return nullptr;

        return &territoryMap[x][y];
    }
    inline Pixel *GetPixelOnMouse() {
        return PixelAt(
            static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).x),
            static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).y)
        );
    }
}

#endif //GLOBALS_H
