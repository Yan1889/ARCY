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
    if (x > 0) neighborsCached.emplace_back(&G::territoryMap[x - 1][y]); // Left
    if (x + 1 < G::MAP_WIDTH) neighborsCached.emplace_back(&G::territoryMap[x + 1][y]); // Right
    if (y > 0) neighborsCached.emplace_back(&G::territoryMap[x][y - 1]); // Up
    if (y + 1 < G::MAP_HEIGHT) neighborsCached.emplace_back(&G::territoryMap[x][y + 1]); // Down
}

const std::vector<Pixel *> &Pixel::GetNeighbors() const {
    return neighborsCached;
}


Color Pixel::GetColor() const {
    return static_cast<const Color *>(G::perlin.data)[G::perlin.width * y + x];
}

float Pixel::GetColorProbability() const {
    const Color terrainColor = GetColor();
    for (const Gradient &mapPart: G::mapParts) {
        if (terrainColor.r == mapPart.color.r &&
            terrainColor.g == mapPart.color.g &&
            terrainColor.b == mapPart.color.b &&
            terrainColor.a == mapPart.color.a) {
            return mapPart.difficulty;
        }
    }
    std::cerr << "Map not correct!" << std::endl;
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
    const float multiplier = contaminated ? 3.f : 1.f;
    const float randomValue = static_cast<float>(rand()) / RAND_MAX;
    return invasionAcceptProbability > randomValue * multiplier;
}