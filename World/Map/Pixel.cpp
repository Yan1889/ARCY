//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include <iostream>
#include <ostream>

#include "../Globals.h"

Pixel::Pixel(const int x, const int y, const int id): x(x), y(y), playerId(id) {
}


void Pixel::LoadNeighbors() {
    loaded = true;
    if (x > 0) neighborsCached.emplace_back(&G::territoryMap[x - 1][y]); // Up
    if (x + 1 < G::WIDTH) neighborsCached.emplace_back(&G::territoryMap[x + 1][y]); // Down
    if (y > 0) neighborsCached.emplace_back(&G::territoryMap[x][y - 1]); // Left
    if (y + 1 < G::HEIGHT) neighborsCached.emplace_back(&G::territoryMap[x][y + 1]); // Right
}

const std::vector<Pixel *>& Pixel::GetNeighbors() {
    if (!loaded) LoadNeighbors();
    return neighborsCached;
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
