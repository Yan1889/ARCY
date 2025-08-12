//
// Created by yanam on 12.08.2025.
//

#ifndef TERRAINKIND_H
#define TERRAINKIND_H

#include "raylib.h"

struct Pixel;

namespace Terrain {
    struct Gradient {
        int level;
        Color color;
        float invasionProbability; // between 0 and 1, 0 = impossible to occupy
    };

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

    enum Kind {
        DEEP_WATER,
        LOW_WATER,
        BEACH,
        OPEN_FIELD,
        HILLS,
        FORREST,
        STONE,
        MOUNTAIN,
        SNOW,
    };

    Kind GetLastKind();

    Color GetColor(Kind kind);

    float GetLevel(Kind kind);

    float GetInvasionProbability(Kind kind);

    Kind GetKindAt(int x, int y);

    Pixel* FindRandomPixelWithKind(Kind kind);
}


#endif //TERRAINKIND_H
