//
// Created by minhp on 21.07.2025.
//

#include "DayNightCycle.h"

#include <cmath>
#include <iostream>
#include <ostream>

#include "raylib.h"

Color DayNightCycle::brightness = {0, 0, 0, 0};
float DayNightCycle::time = 0;
int DayNightCycle::days = 0;
bool DayNightCycle::daysCounting = true;

void DayNightCycle::Draw()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), brightness);
    Time();
}

void DayNightCycle::Update()
{
    time += GetFrameTime() / 300.0f; // Default 300.0f

    if (time > 1.0f)
    {
        time = 0.0f;
        daysCounting = true;
    }
    if (0.5 < time && time < 0.75 && daysCounting)
    {
        daysCounting = false;
        days++;
    }

    float alpha = (sinf(time * 2 * PI - PI / 2) + 1) / 2;
    unsigned char nightAlpha = (unsigned char)(alpha * 245);

    brightness = { 0, 0, 0, nightAlpha };
    Draw();
}

void DayNightCycle::Time()
{
    float convertedTime = fmod(time + 0.5f, 1.0f) * 24.0f;

    int hour = static_cast<int>(convertedTime);
    int minute = static_cast<int>((convertedTime - hour) * 60);

    char timeString[16];
    sprintf(timeString, "%02d:%02d", hour, minute);
    DrawText(timeString, GetScreenWidth() - 100, 25, 20, WHITE);
    const char* dayString = TextFormat("Day %d", days);
    DrawText(dayString, GetScreenWidth() - 100, 50, 20, WHITE );
}
