//
// Created by yanam on 12.07.2025.
//

#ifndef PIXELREF_H
#define PIXELREF_H
#include "Pixel.h"


struct PixelRef {
    int x, y;

    [[nodiscard]] std::vector<PixelRef> GetNeighborPixels() const;

    bool operator<(const PixelRef &other) const;

    bool operator==(const PixelRef &other) const;

    void operator+=(const PixelRef &other);

    // explicit operator Vector2() const;

    struct Hasher {
        std::size_t operator()(const PixelRef &p) const {
            std::size_t seed = std::hash<int>()(p.x);
            seed ^= std::hash<int>()(p.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };


    Pixel& GetActual() const;
    Color GetColor() const;
    int GetPlayerId() const;
};



#endif //PIXELREF_H
