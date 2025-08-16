//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>

#include "raylib.h"
#include "TerrainKind.h"


struct Pixel {
    int x{};
    int y{};
    int idx{};
    int playerId{};
    Terrain::Kind kind{};

    bool loaded{};
    bool contaminated{};
    std::vector<Pixel *> neighborsCached{};

    Pixel() = default;
    Pixel(int x, int y);

    void Load(const Color &color);
    void LoadNeighbors();

    [[nodiscard]] const std::vector<Pixel *>& GetNeighbors() const;
    [[nodiscard]] bool acceptRandomly() const;
    [[nodiscard]] Vector2 ToVector2() const;
};

#endif //PIXEL_H
