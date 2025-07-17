//
// Created by yanam on 16.07.2025.
//

#ifndef CITY_H
#define CITY_H
#include "Building.h"


struct Pixel;

class City: public Building {
public:
    explicit City(Pixel* pos);
};



#endif //CITY_H
