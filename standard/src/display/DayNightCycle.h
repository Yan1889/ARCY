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
    static void Time();
    static Color brightness;
    static float time;
    static int days;
    static bool daysCounting;
    static float smoothTransition(float x, float y, float z);
};



#endif //DAYNIGHTCYCLE_H
