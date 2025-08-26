//
// Created by yanam on 20.07.2025.
//

#ifndef BUILDING_H
#define BUILDING_H
#include <iostream>

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

    inline int GetCost(const BUILDING_TYPE type) {
        switch (type) {
            case CITY: return cityCost;
            case SILO: return siloCost;
            case UNKNOWN: std::cout << "[Error]" << std::endl;
        }
        return -1;
    }
}

#endif //BUILDING_H
