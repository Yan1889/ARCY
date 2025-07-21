//
// Created by minhp on 21.07.2025.
//

#include "DayNightCycle.h"

#include <cmath>

#include "raylib.h"

Color DayNightCycle::brightness = {0, 0, 0, 0};
float DayNightCycle::time = 0;
int DayNightCycle::days = 0;

void DayNightCycle::Draw()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), brightness);
}

void DayNightCycle::Update()
{
    time += GetFrameTime() / 300.0f; // Default 300.0f
    if (time > 1.0f)
    {
        time = 0.0f;
        days++;
    }

    float alpha = (sinf(time * 2 * PI - PI / 2) + 1) / 2;
    unsigned char nightAlpha = (unsigned char)(alpha * 245);

    brightness = { 0, 0, 0, nightAlpha };
    Draw();
}
