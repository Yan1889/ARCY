//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>
#include <cmath>

#include "Globals.h"

struct Pixel {
    int x;
    int y;
    int playerId;

    [[nodiscard]] std::vector<Pixel> GetNeighborPixels() const {
        std::vector<Pixel> result;

        if (y > 0) result.push_back(G::territoryMap[y - 1][x]);         // Up
        if (y + 1 < G::HEIGHT) result.push_back(G::territoryMap[y + 1][x]); // Down
        if (x > 0) result.push_back(G::territoryMap[y][x - 1]);         // Left
        if (x + 1 < G::WIDTH) result.push_back(G::territoryMap[y][x + 1]); // Right

        return result;
    }

    bool operator<(const Pixel &other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    bool operator==(const Pixel &other) const {
        return x == other.x && y == other.y;
    }

    struct Hasher {
        std::size_t operator()(const Pixel &p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
};

#endif //PIXEL_H
