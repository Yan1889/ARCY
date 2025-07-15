//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>
#include <cmath>

#include "raylib.h"


struct Pixel {
    int x;
    int y;
    int playerId;

    float invasionAcceptProbability;
    bool queuedUpForAttack = false;
    std::vector<Pixel *> neighborsCached;


    Pixel() = default;
    Pixel(int x, int y, int id);

    void LoadNeighbors();

    const std::vector<Pixel *>& GetNeighbors();

    bool operator<(const Pixel &other) const;

    bool operator==(const Pixel &other) const;

    Color GetColor() const;

    float GetColorProbability() const;

    struct Hasher {
        std::size_t operator()(const Pixel &p) const {
            std::size_t seed = std::hash<int>()(p.x);
            seed ^= std::hash<int>()(p.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

};

#endif //PIXEL_H
