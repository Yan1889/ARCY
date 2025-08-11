//
// Created by yanam on 04.07.2025.
//

#ifndef PIXEL_H
#define PIXEL_H

#include <vector>

#include "raylib.h"


struct Pixel {
    int x{};
    int y{};
    int playerId{};

    float invasionAcceptProbability{};
    bool contaminated{};
    std::vector<Pixel *> neighborsCached;

    Pixel() = default;
    Pixel(int x, int y, int id);

    void LoadNeighbors();

    [[nodiscard]] const std::vector<Pixel *>& GetNeighbors() const;
    [[nodiscard]] bool acceptRandomly() const;
    [[nodiscard]] Color GetColor() const;
    [[nodiscard]] float GetColorProbability() const;
    [[nodiscard]] Vector2 ToVector2() const;
};

#endif //PIXEL_H
