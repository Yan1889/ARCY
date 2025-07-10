//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include <vector>

#include "Globals.h"


Pixel::Pixel(const int x, const int y, const int id): x(x), y(y), playerId(id) {
}

Pixel::Pixel(const Vector2 vec): x(static_cast<int>(vec.x)), y(static_cast<int>(vec.y)), playerId(-1) {
}

std::vector<Pixel> Pixel::GetNeighborPixels() const {
    std::vector<Pixel> result;

    if (y > 0) result.push_back(G::territoryMap[y - 1][x]); // Up
    if (y + 1 < G::HEIGHT) result.push_back(G::territoryMap[y + 1][x]); // Down
    if (x > 0) result.push_back(G::territoryMap[y][x - 1]); // Left
    if (x + 1 < G::WIDTH) result.push_back(G::territoryMap[y][x + 1]); // Right

    return result;
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

Pixel::operator Vector2() const {
    return Vector2{
        static_cast<float>(x),
        static_cast<float>(y)
    };
}
