#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "World/PerlinNoise.h"
#include "World/TextureCollection.h"
#include "World/Population.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650

#define MAP_HEIGHT 2500
#define MAP_WIDTH 2500



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



const int moveSpeed = 100;
const int zoomSpeed = 5;

const float zoomMin = 0.1f;
const float zoomMax = 10.0f;


Vector2 playerPos(MAP_WIDTH / 2, MAP_HEIGHT / 2);
Camera2D camera{};

Image perlin;
Texture2D perlinTexture {0};

void handleCamera();
void displayFps();
void displayUserInstructions();
void checkExplosion();


int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");
    TextureCollection::LoadAll();

    // we dont need 4000 fps - Yes we need 4000+ fps to ditch Python! Sincerely Colin
    SetTargetFPS(60);


    // Camera
    camera.target = playerPos;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    perlin = GenImagePerlinNoise(
        MAP_WIDTH, MAP_HEIGHT,
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (SCREEN_WIDTH / 2),
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (SCREEN_HEIGHT / 2),
        6
    );

    std::vector<std::vector<float> > falloff = PerlinNoise::GenerateFalloffMap(MAP_WIDTH, MAP_HEIGHT);

    PerlinNoise::ApplyFalloffToImage(&perlin, falloff); // finally use falloff

    PerlinNoise::proceedMap(&perlin, map);

    perlinTexture = LoadTextureFromImage(perlin);

    const Vector2 centerPos = playerPos;

    std::vector<Vector2> circlePositions{};

    std::cout << Population::population << std::endl;

    while (!WindowShouldClose()) {
        handleCamera();
        checkExplosion();

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Create cities when left-clicking
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            circlePositions.emplace_back(GetScreenToWorld2D(GetMousePosition(), camera));
            Population::cities++;
        }

        BeginDrawing();

        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        DrawCircleV(centerPos, 6 * 1/camera.zoom, RED);


        // display each city (now fr)
        for (const Vector2 &circle : circlePositions) {
            DrawTextureV(TextureCollection::city, circle, WHITE);
        }

        // Population
        Population::Update();

        EndMode2D();

        displayFps();

        const char *populationText = TextFormat("Population: %d / %d", Population::population, Population::maxPopulation);
        DrawText(populationText, 0 + 25, GetScreenHeight() - 25, 20, DARKGREEN);

        displayUserInstructions();

        EndDrawing();
    }

    TextureCollection::UnloadAll();
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
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, DARKGREEN);

    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20, DARKGREEN);

    DrawText("Left-click to build a city", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80, 20, DARKGREEN);

    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, DARKGREEN);

    DrawText("Right-click to drop a bomb", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 160, 20, DARKGREEN);

    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20, DARKGREEN);
}

void handleCamera() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;

    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();

    if (playerPos.x > 3000) playerPos.x = 3000;
    else if (playerPos.x < -1000) playerPos.x = -1000;

    if (playerPos.y > 3000) playerPos.y = 3000;
    else if (playerPos.y < -1000) playerPos.y = -1000;

    camera.target = playerPos;

    if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed * GetFrameTime();

    if (camera.zoom < zoomMin) camera.zoom = zoomMin;
    if (camera.zoom > zoomMax) camera.zoom = zoomMax;
}

void checkExplosion()
{
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        int radius = 50;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int)mousePos.x + x;
                int py = (int)mousePos.y + y;

                if (x * x + y * y <= radius * radius) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {
                        Color explosionColor = Color(0, 255, 0, 255);

                        ImageDrawPixel(&perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(perlinTexture);
        perlinTexture = LoadTextureFromImage(perlin);
    }
}
