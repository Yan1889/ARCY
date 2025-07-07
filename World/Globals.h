//
// Created by yanam on 02.07.2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <vector>

#include "PerlinNoise.h"
#include "Pixel.h"
#include "raylib.h"

struct Gradient;

class Player;

// G for global
namespace G {
    inline Image perlin;
    inline Texture2D perlinTexture{0};
    inline std::vector<Gradient> mapParts = {
        Gradient{32 * 1 + 10, {90, 90, 255, 255}, 0.f}, // Deep Water
        Gradient{32 * 2 + 10, {125, 125, 255, 255}, 0.f}, // Low Water
        Gradient{32 * 2 + 20, {247, 252, 204, 255}, 0.5f}, // Beach
        Gradient{32 * 3 + 20, {129, 245, 109, 255}, 1.0f}, // Open Field
        Gradient{32 * 4 + 10, {117, 219, 99, 255}, 0.7f}, // Hills
        Gradient{32 * 5 + 20, {97, 184, 81, 255}, 0.3f}, // Forest
        Gradient{32 * 6, {191, 191, 191, 255}, 0.1f}, // Stone
        Gradient{32 * 7 - 20, {153, 153, 153, 255}, 0.f}, // Mountain
        Gradient{32 * 8, {255, 255, 255, 255}, 0.f}, // Snow
    };


    inline float maxDifficulty = 30;

    inline int playerCount;

    inline int WIDTH;
    inline int HEIGHT;
    inline std::vector<std::vector<Pixel> > territoryMap{}; // [y][x]
    inline Texture2D territoryTexture;
    inline Image territoryImage;


    inline void InitMap(const int w, const int h) {
        WIDTH = w;
        HEIGHT = h;
    }
}

#endif //GLOBALS_H
