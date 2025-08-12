//
// Created by yanam on 02.07.2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <vector>
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

    inline Sounds mySounds;

    inline float maxDifficulty = 30;

    inline std::vector<Player> players;
    inline Vector2 playerPos;

    inline bool gameOver{};
    inline int winnerId = -1;

    inline Image explosionImage;
    inline Texture2D explosionTexture;
    inline bool explosionTextureDirty{};

    inline std::vector<std::vector<Pixel> > territoryMap; // [x][y]
    inline Texture2D territoryTexture;
    inline Image territoryImage;
    inline bool territoryTextureDirty{};

    inline Pixel* PixelAt(const int x, const int y) {
        if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT) return nullptr;
        return &territoryMap[x][y];
    }
    inline Pixel* PixelAt(Vector2 v) {
        const int x = static_cast<int>(v.x);
        const int y = static_cast<int>(v.y);

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
