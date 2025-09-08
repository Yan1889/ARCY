//
// Created by yanam on 07.07.2025.
//

#include "Pixel.h"

#include <iostream>


#include "../Globals.h"

Pixel::Pixel(const int x, const int y): x(x), y(y), playerId(-1) {
}

void Pixel::Load(const Color &color) {
    if (!loaded) {
        loaded = true;
        this->kind = Terrain::GetKindFromColor(color);
    } else {
        //std::cerr << "already loaded pixel\n"; GET OUT!!!
    }
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

bool Pixel::AcceptRandomly() const {
    if (!loaded) {
        std::cerr << "[Error] didnt load pixel\n";
    }
    // radiation = 3x harder
    const float multiplier = contaminated ? 3.f : 1.f;
    const float randomValue = G::RandomFloat_0to1();
    return Terrain::GetInvasionProbability(kind) > randomValue * multiplier;
}
