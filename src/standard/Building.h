//
// Created by yanam on 20.07.2025.
//

#ifndef BUILDING_H
#define BUILDING_H

struct Pixel;

enum BUILDING_TYPE {
    CITY,
    SILO,
    UNKNOWN
};

struct Building {
    Pixel *pos;
    BUILDING_TYPE type;
};

#endif //BUILDING_H
