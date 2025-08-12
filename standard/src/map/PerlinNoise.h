//
// Created by yanam on 18.06.2025.
//

#ifndef PERLINNOISE_H
#define PERLINNOISE_H
#include <vector>

#include "raylib.h"

class PerlinNoise {
public:
    static void proceedMap(Image *image);

    static void ApplyFalloffToImage(Image *image, const std::vector<std::vector<float> > &falloffMap);

    static std::vector<std::vector<float> > GenerateFalloffMap(int width, int height);

    static float FalloffValue(float x, float y);
};


#endif //PERLINNOISE_H
