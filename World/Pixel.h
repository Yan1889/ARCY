//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>
#include <cmath>


struct Pixel {
    int x;
    int y;
    int playerId;

    [[nodiscard]] std::vector<Pixel> GetNeighborPixels() const;

    bool operator<(const Pixel &other) const;

    bool operator==(const Pixel &other) const;

    void operator+=(const Pixel &other);

    struct Hasher {
        std::size_t operator()(const Pixel &p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
};

#endif //PIXEL_H
