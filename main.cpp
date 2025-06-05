#include <iostream>
#include "raylib.h"
#include "raymath.h"

int main() {
    InitWindow(800, 450, "raylib [core] example - basic window");

    SetTargetFPS(60);

    Image ant = LoadImage("images/ant.png");
    Texture2D antTexture = LoadTextureFromImage(ant);

    Vector2 mousePos = GetMousePosition();
    Vector2 antPos = mousePos;
    float lerpAmount = 0.22f;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        mousePos = GetMousePosition();
        antPos = Vector2Lerp(antPos, mousePos, lerpAmount);

        DrawTextureV(antTexture, antPos, WHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        DrawFPS(GetScreenWidth() - 100, GetScreenHeight() - 25);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}