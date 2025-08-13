//
// Created by minhp on 21.07.2025.
//

#include "DayNightCycle.h"

#include <cmath>
#include <iostream>
#include <ostream>

#include "raylib.h"
#include "../Globals.h"

using namespace G;

Color DayNightCycle::brightness = {0, 0, 0, 0};
float DayNightCycle::time = 0;
int DayNightCycle::days = 0;
bool DayNightCycle::daysCounting = true;
bool DayNightCycle::isNightTime = time > 0.25f && time < 0.75f ? false : true;

void DayNightCycle::Draw()
{
    Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D({static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, camera);
    DrawRectangleV(topLeft,
            {bottomRight.x - topLeft.x, bottomRight.y - topLeft.y}, brightness);
}

float DayNightCycle::smoothTransition(float x, float y, float z)
{
    z = (z - x) / (y - x);
    z = z < 0 ? 0 : (z > 1 ? 1 : z);
    return z * z * (3 - 2 * z);
}

void DayNightCycle::Update()
{
    time += GetFrameTime() / 750.0f; // Default 300.0f

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

    if (time > 0.3f && time < 0.70f) isNightTime = true;
    else isNightTime = false;

    float alpha = (sinf(time * 2 * PI - PI / 2) + 1) / 2;
    unsigned char nightAlpha = (unsigned char)(alpha * 250);

    Color overlayColor;

    // Evening
    if (time >= 0.20f && time <= 0.30f) {
        float t = (time - 0.20f) / 0.10f;
        float colorFade = t <= 0.5f ? t * 2.0f : (1.0f - t) * 2.0f;
        colorFade = smoothTransition(0.0f, 1.0f, colorFade);

        overlayColor = {
            (unsigned char)(255 * colorFade),
            (unsigned char)(150 * colorFade),
            0,
            (unsigned char)(180 * t)
        };
    }
    // Night
    else if (time > 0.30f && time < 0.50f) {
        float t = (time - 0.30f) / 0.20f;
        overlayColor = {0, 0, 0, (unsigned char)(180 + t * (nightAlpha - 180))};
    }
    // Midnight
    else if (time >= 0.50f && time <= 0.60f) {
        overlayColor = {0, 0, 0, nightAlpha};
    }
    // Morning
    else if (time > 0.60f && time < 0.70f) {
        float t = (time - 0.60f) / 0.10f;
        float colorFade = t <= 0.5f ? t * 2.0f : (1.0f - t) * 2.0f;
        colorFade = smoothTransition(0.0f, 1.0f, colorFade);

        overlayColor = {
            (unsigned char)(255 * colorFade),
            (unsigned char)(150 * colorFade),
            0,
            (unsigned char)(180 * (1 - t))
        };
    }

    brightness = overlayColor;

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
