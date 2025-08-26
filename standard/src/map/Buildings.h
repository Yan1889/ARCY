//
// Created by yanam on 20.07.2025.
//

#ifndef BUILDING_H
#define BUILDING_H

#include "../loaders/TextureCollection.h"

struct Pixel;

namespace Buildings {
    enum BUILDING_TYPE {
        CITY,
        SILO,
        UNKNOWN
    };

    struct Building {
        Pixel *pos;
        BUILDING_TYPE type;
    };

    inline int cityCost = 10'000;
    inline int siloCost = 1'000'000;

    int GetCost(BUILDING_TYPE type);

    Texture2D &GetBuildingTexture(BUILDING_TYPE t);
}

#endif //BUILDING_H
