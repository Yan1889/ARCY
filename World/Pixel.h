//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>
#include <cmath>

#include "Globals.h"
/*
struct Pixel {
    int x;
    int y;
    int playerId;

    [[nodiscard]] std::vector<Pixel> GetNeighborPixels() const {
        std::vector<Pixel> result;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (std::abs(dx) == std::abs(dy)) continue;

                result.push_back(G::territoryMap[y + dy][x + dx]);
            }
        }
        return result;
    }

    bool operator<(const Pixel& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    bool operator==(const Pixel& other) const {
        return x == other.x && y == other.y;
    }

    struct Hasher {
        std::size_t operator()(const Pixel& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
};
*/

struct Pixel {
    int x;
    int y;
    int playerId;

    [[nodiscard]] std::vector<Pixel> GetNeighborPixels() const {
        std::vector<Pixel> result;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (std::abs(dx) == std::abs(dy)) continue;

                result.push_back(G::territoryMap[y + dy][x + dx]);
            }
        }
        return result;
    }

    bool operator<(const Pixel& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    bool operator==(const Pixel& other) const {
        return x == other.x && y == other.y;
    }

    struct Hasher {
        std::size_t operator()(const Pixel& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
};

#endif //PIXEL_H
