//
// Created by yanam on 02.07.2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <vector>

struct Gradient;

// G for global
namespace G {
    inline std::vector<Gradient> map = {
        {32 * 1 + 10, Color{90, 90, 255, 255}, -1}, // Deep Water
        {32 * 2 + 10, Color{125, 125, 255, 255}, -1}, // Low Water
        {32 * 2 + 20, Color{247, 252, 204, 255}, 10}, // Beach
        {32 * 3 + 20, Color{129, 245, 109, 255}, 0}, // Open Field
        {32 * 4 + 10, Color{117, 219, 99, 255}, 3}, // Hills
        {32 * 5 + 20, Color{97, 184, 81, 255}, 15}, // Forest
        {32 * 6, Color{191, 191, 191, 255}, 30}, // Stone
        {32 * 7 - 20, Color{153, 153, 153, 255}, -1}, // Mountain
        {32 * 8, Color{255, 255, 255, 255}, -1}, // Snow
    };
    inline float maxDifficulty = 30;
}
#endif //GLOBALS_H
