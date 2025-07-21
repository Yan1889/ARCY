//
// Created by minhp on 21.07.2025.
//

#ifndef DAYNIGHTCYCLE_H
#define DAYNIGHTCYCLE_H

#include "raylib.h"


class DayNightCycle {
public:
    static void Update();
    static void Draw();
    static Color brightness;
    static float time;
    static int days;
};



#endif //DAYNIGHTCYCLE_H
