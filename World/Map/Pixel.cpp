//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include "../Globals.h"


std::vector<Pixel *> Pixel::GetNeighborPixels() const {
    std::vector<Pixel *> result;

    if (x > 0) result.emplace_back(&G::territoryMap[x - 1][y]); // Up
    if (x + 1 < G::WIDTH) result.emplace_back(&G::territoryMap[x + 1][y]); // Down
    if (y > 0) result.emplace_back(&G::territoryMap[x][y - 1]); // Left
    if (y + 1 < G::HEIGHT) result.emplace_back(&G::territoryMap[x][y + 1]); // Right

    return result;
}

Color Pixel::GetColor() const {
    return static_cast<const Color *>(G::perlin.data)[G::perlin.width * y + x];
}

bool Pixel::operator<(const Pixel &other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
}

bool Pixel::operator==(const Pixel &other) const {
    return x == other.x && y == other.y;
}

void Pixel::operator+=(const Pixel &other) {
    x += other.x;
    y += other.y;
}
