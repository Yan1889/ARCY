//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include <algorithm>
#include <iostream>

#include "../Globals.h"

Pixel::Pixel(const int x, const int y, const Terrain::Kind kind): x(x),
                                                                  y(y),
                                                                  idx(G::ToIdx(x, y)),
                                                                  playerId(-1),
                                                                  kind(kind) {
}

void Pixel::LoadNeighbors() {
    const std::vector<Pixel *> allNeighbors = {
        G::PixelAt(x - 1, y), // left
        G::PixelAt(x + 1, y), // right
        G::PixelAt(x, y - 1), // up
        G::PixelAt(x, y + 1), // down
    };
    for (Pixel *n: allNeighbors) {
        if (n) {
            neighborsCached.push_back(n);
        }
    }
}

const std::vector<Pixel *> &Pixel::GetNeighbors() const {
    return neighborsCached;
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
    const float randomValue = G::RandomFloat_0to1(); // static_cast<float>(rand()) / RAND_MAX;
    return Terrain::GetInvasionProbability(kind) > randomValue * multiplier;
}
