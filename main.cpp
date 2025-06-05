#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

struct Gradient {
    int level;
    Color color;
};

Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}

void proceedMap(Image *image, const std::vector<struct Gradient> &array) {
    for (int i = 0; i < array[0].level && i < 256; i++) {
        ImageColorReplace(image, grayScale(i), array[0].color);
    }
    for (int j = 1; j < array.size(); j++) {
        for (int i = array[j - 1].level; i < array[j].level && i < 256; i++) {
            ImageColorReplace(image, grayScale(i), array[j].color);
        }
    }
}

int main() {
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");

    SetTargetFPS(60);

    Image perlin = GenImagePerlinNoise(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_WIDTH/2), static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_HEIGHT/2), 1);

    std::vector<struct Gradient> map = {
{32 * 1, Color{90, 90, 255, 255}},
{32 * 2, Color{125, 125, 255, 255}},
{32*2+10, Color{247, 252, 204, 255}},
{32 * 3, Color{129, 245, 109, 255}},
{32 * 4, Color{117, 219, 99, 255}},
{32 * 5, Color{97, 184, 81, 255}},
{32 * 6, Color{191, 191, 191, 255}},
{32*7-20, Color{153, 153, 153, 255}},
{32 * 8, Color{255, 255, 255, 255}},
    };

    proceedMap(&perlin, map);

    ImageResizeNN(&perlin, SCREEN_WIDTH, SCREEN_HEIGHT);

    Texture2D perlinTexture = LoadTextureFromImage(perlin);

    Image ant = LoadImage("images/ant.png");
    Texture2D antTexture = LoadTextureFromImage(ant);

    Vector2 mousePos = GetMousePosition();
    Vector2 antPos = mousePos;
    float lerpAmount = 0.22f;

    while (!WindowShouldClose()) {
        BeginDrawing();

        // ClearBackground(RAYWHITE);
        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        mousePos = GetMousePosition();
        antPos = Vector2Lerp(antPos, mousePos, lerpAmount);

        DrawTextureV(antTexture, antPos, WHITE);

        DrawFPS(GetScreenWidth() - 100, GetScreenHeight() - 25);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}