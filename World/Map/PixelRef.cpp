//
// Created by yanam on 12.07.2025.
//

#include "PixelRef.h"

#include "../Globals.h"

Pixel &PixelRef::GetActual() const {
    return G::territoryMap[y][x];
}

int PixelRef::GetPlayerId() const {
    return GetActual().playerId;
}

Color PixelRef::GetColor() const {
    return static_cast<const Color *>(G::perlin.data)[G::perlin.width * y + x];
}

std::vector<PixelRef> PixelRef::GetNeighborPixels() const {
    std::vector<PixelRef> result;

    if (y > 0) result.emplace_back(y - 1, x); // Up
    if (y + 1 < G::HEIGHT) result.emplace_back(y + 1, x); // Down
    if (x > 0) result.emplace_back(y, x - 1); // Left
    if (x + 1 < G::WIDTH) result.emplace_back(y, x + 1); // Right

    return result;
}

bool PixelRef::operator<(const PixelRef &other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
}

bool PixelRef::operator==(const PixelRef &other) const {
    return x == other.x && y == other.y;
}

void PixelRef::operator+=(const PixelRef &other) {
    x += other.x;
    y += other.y;
}


/*
*Pixel::operator Vector2() const {
return Vector2{
static_cast<float>(x),
static_cast<float>(y)
};
}
*/
