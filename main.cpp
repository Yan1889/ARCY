#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "PerlinNoise/PerlinNoise.h"

#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 650

#define MAP_HEIGHT 2500
#define MAP_WIDTH 2500

#define MAX_CIRCLES 50


Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}

const std::vector<Gradient> map = {
    {32 * 1 + 10, Color{90, 90, 255, 255}}, // Deep Water
    {32 * 2 + 10, Color{125, 125, 255, 255}}, // Low Water
    {32 * 2 + 20, Color{247, 252, 204, 255}}, // Beach
    {32 * 3 + 20, Color{129, 245, 109, 255}}, // Open Field
    {32 * 4 + 10, Color{117, 219, 99, 255}}, // Hills
    {32 * 5 + 20, Color{97, 184, 81, 255}}, // Forest
    {32 * 6, Color{191, 191, 191, 255}}, // Stone
    {32 * 7 - 20, Color{153, 153, 153, 255}}, // Mountain
    {32 * 8, Color{255, 255, 255, 255}}, // Snow
};


const float lerpAmount = 0.88f;

const int moveSpeed = 100;
const int zoomSpeed = 5;

const float zoomMin = 0.1f;
const float zoomMax = 10.0f;


Vector2 playerPos(MAP_WIDTH / 2, MAP_HEIGHT / 2);
Camera2D camera{};


void handleCamera();
void displayFps();
void displayUserInstructions();


int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");

    // we dont need 4000 fps
    SetTargetFPS(60);


    // Camera
    camera.target = playerPos;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Image perlin = GenImagePerlinNoise(
        MAP_WIDTH, MAP_HEIGHT,
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (SCREEN_WIDTH / 2),
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (SCREEN_HEIGHT / 2),
        6
    );

    std::vector<std::vector<float> > falloff = PerlinNoise::GenerateFalloffMap(MAP_WIDTH, MAP_HEIGHT);

    PerlinNoise::ApplyFalloffToImage(&perlin, falloff); // finally use falloff

    PerlinNoise::proceedMap(&perlin, map);

    const Texture2D perlinTexture = LoadTextureFromImage(perlin);

    const Image ant = LoadImage("images/ant.png");
    const Texture2D antTexture = LoadTextureFromImage(ant);

    Vector2 mousePos = GetMousePosition();
    Vector2 antPos = mousePos;

    const Vector2 centerPos = playerPos;

    Vector2 circles[MAX_CIRCLES];
    int circleCount = 0;

    while (!WindowShouldClose()) {
        handleCamera();

        mousePos = GetScreenToWorld2D(GetMousePosition(), camera);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && circleCount < MAX_CIRCLES) // Create "cities" when left clicking
        {
            circles[circleCount] = mousePos;
            circleCount++;
        }

        BeginDrawing();

        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        antPos = Vector2Lerp(antPos, mousePos, lerpAmount * GetFrameTime());

        DrawTextureV(antTexture, antPos, WHITE);

        DrawCircleV(centerPos, 10, RED);

        for (int i = 0; i < circleCount; i++) // Display "cities"
        {
            DrawCircleV(circles[i], 20, BLUE);
        }

        EndMode2D();

        displayFps();
        displayUserInstructions();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void displayFps() {
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);

    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;

    DrawText(fpsText, textX, textY, 20, DARKGREEN);
}

void displayUserInstructions() {
    const char *controlsText = TextFormat("Move with WASD");

    DrawText(controlsText, GetScreenWidth() / 20, GetScreenHeight() / 20, 20, DARKGREEN);

    const char *controlsText1 = TextFormat("Left or Right Arrow to zoom");

    DrawText(controlsText1, GetScreenWidth() / 20, GetScreenHeight() / 20 + 40, 20, DARKGREEN);

    const char *controlsText2 = TextFormat("Leftclick to build a 'city'");

    DrawText(controlsText2, GetScreenWidth() / 20, GetScreenHeight() / 20 + 80, 20, DARKGREEN);
}

void handleCamera() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;

    camera.target = playerPos;

    if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed * GetFrameTime();

    if (camera.zoom < zoomMin) camera.zoom = zoomMin;
    if (camera.zoom > zoomMax) camera.zoom = zoomMax;
}
