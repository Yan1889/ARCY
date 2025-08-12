//
// Created by yanam on 12.08.2025.
//

#include "../Globals.h"
#include "TerrainKind.h"

#include <iostream>

Terrain::Kind Terrain::GetLastKind() {
    return static_cast<Kind>(mapParts.size() - 1);
}

Color Terrain::GetColor(const Kind kind) {
    return mapParts[kind].color;
}

float Terrain::GetLevel(const Kind kind) {
    return mapParts[kind].level;
}

float Terrain::GetInvasionProbability(const Kind kind) {
    return mapParts[kind].invasionProbability;
}

Terrain::Kind Terrain::GetKindAt(const int x, const int y) {
    const Color c = static_cast<const Color *>(G::perlin.data)[G::perlin.width * y + x];
    for (int i = 0; i < mapParts.size(); i++) {
        const Color g = mapParts[i].color;
        if (c.r == g.r && c.g == g.g && c.b == g.b && c.a == g.a) {
            return static_cast<Kind>(i);
        }
    }
}

Pixel *Terrain::FindRandomPixelWithKind(const Kind kind) {
    while (true) {
        const int x = rand() % G::MAP_WIDTH;
        const int y = rand() % G::MAP_HEIGHT;
        if (GetKindAt(x, y) == kind) {
            return G::PixelAt(x, y);
        }
    }
}
