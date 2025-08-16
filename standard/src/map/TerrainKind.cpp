//
// Created by yanam on 12.08.2025.
//

#include "../Globals.h"
#include "../map/ChunkGeneration.h"
#include "TerrainKind.h"

#include <iostream>

Terrain::Kind Terrain::GetLastKind() {
    return static_cast<Kind>(mapParts.size() - 1);
}

Color Terrain::GetColor(const Kind kind) {
    return mapParts[kind].color;
}

int Terrain::GetLevel(const Kind kind) {
    return mapParts[kind].level;
}

float Terrain::GetInvasionProbability(const Kind kind) {
    return mapParts[kind].invasionProbability;
}


Terrain::Kind Terrain::GetKindFromColor(const Color &c) {
    for (int i = 0; i < mapParts.size(); i++) {
        const Color g = mapParts[i].color;
        if (c.r == g.r && c.g == g.g && c.b == g.b && c.a == g.a) {
            return static_cast<Kind>(i);
        }
    }
    std::cerr << "[Error] color does not match to any layer" << std::endl;
    return SNOW;
}

Pixel *Terrain::FindRandomPixelWithKind(const Kind kind) {
    while (true) {
        const int x = rand() % G::MAP_WIDTH;
        const int y = rand() % G::MAP_HEIGHT;
        Pixel *p = G::PixelAt(x, y);

        if (p->loaded && p->kind == kind) {
            return p;
        }
    }
}
