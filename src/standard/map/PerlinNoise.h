//
// Created by yanam on 18.06.2025.
//

#ifndef PERLINNOISE_H
#define PERLINNOISE_H
#include <vector>

#include "raylib.h"

struct Gradient {
    int level;
    Color color;
    float difficulty; // between 0 and 1, 0 = impossible to occupy
};

class PerlinNoise {
public:
    static void proceedMap(Image *image, const std::vector<Gradient> &map);

    static void ApplyFalloffToImage(Image *image, const std::vector<std::vector<float> > &falloffMap);

    static std::vector<std::vector<float> > GenerateFalloffMap(int width, int height);

    static float FalloffValue(float x, float y);
};


#endif //PERLINNOISE_H
