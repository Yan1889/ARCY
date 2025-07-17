//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include <iostream>

#include "../Globals.h"

Pixel::Pixel(const int x, const int y, const int id): x(x), y(y), playerId(id),
                                                      invasionAcceptProbability(GetColorProbability()) {
}

void Pixel::LoadNeighbors() {
    if (x > 0) neighborsCached.emplace_back(&G::territoryMap[x - 1][y]); // Up
    if (x + 1 < G::MAP_WIDTH) neighborsCached.emplace_back(&G::territoryMap[x + 1][y]); // Down
    if (y > 0) neighborsCached.emplace_back(&G::territoryMap[x][y - 1]); // Left
    if (y + 1 < G::MAP_HEIGHT) neighborsCached.emplace_back(&G::territoryMap[x][y + 1]); // Right
}

const std::vector<Pixel *> &Pixel::GetNeighbors() {
    return neighborsCached;
}


Color Pixel::GetColor() const {
    return static_cast<const Color *>(G::perlin.data)[G::perlin.width * y + x];
}

float Pixel::GetColorProbability() const {
    Color terrainColor = GetColor();
    for (auto &mapPart: G::mapParts) {
        if (terrainColor.r == mapPart.color.r) {
            return mapPart.difficulty;
        }
    }
    return -1;
}

Vector2 Pixel::ToVector2() const {
    return Vector2{
        static_cast<float>(x),
        static_cast<float>(y)
    };
}

bool Pixel::acceptRandomly() const {
    // radiation = 3x harder
    const bool radiated = GetImageColor(G::explosionImage, x, y).a > 0;
    return invasionAcceptProbability > static_cast<float>(rand()) / RAND_MAX * (radiated ? 3 : 1);
}

bool Pixel::operator<(const Pixel &other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
}

bool Pixel::operator==(const Pixel &other) const {
    return x == other.x && y == other.y;
}
