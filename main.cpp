#include <iostream>
#include <thread>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "World/PerlinNoise.h"
#include "World/TextureCollection.h"
#include "World/Player.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650

#define MAP_HEIGHT 2500
#define MAP_WIDTH 2500


Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}

// ----- perlin settings -----
Image perlin;
Texture2D perlinTexture{0};
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


// ----- camera setting -----
Camera2D camera{};
const int moveSpeed = 100;
const int zoomSpeed = 5;
const float zoomMin = 0.1f;
const float zoomMax = 10.0f;


Vector2 playerPos(MAP_WIDTH / 2, MAP_HEIGHT / 2);
Player *player;


void initCamAndMap();

void handleCamera();

void displayInfoTexts();

void checkExplosion();


int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");
    TextureCollection::LoadAll();

    // we dont need 4000 fps - Yes we need 4000+ fps to ditch Python! Sincerely Colin
    SetTargetFPS(60);

    initCamAndMap();

    player = new Player(
        {
            static_cast<int>(playerPos.x),
            static_cast<int>(playerPos.y)
        },
        10
    );

    std::cout << player->_population << std::endl;

    while (!WindowShouldClose()) {
        handleCamera();
        checkExplosion();

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Create cities when left-clicking
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            player->AddCity(GetScreenToWorld2D(GetMousePosition(), camera));
        }

        // expand with space is clicked
        if (IsKeyPressed(KEY_SPACE)) {
            player->Expand(0.5);
        }

        BeginDrawing();

        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);


        for (const Pixel &pixel: player->_frontierPixels) {
            DrawPixel(pixel.x, pixel.y, RED);
        }

        for (const Pixel &pixel: player->_allPixels) {
            DrawPixel(pixel.x, pixel.y, Fade(ORANGE, 0.5));
        }

        // display each city (now fr)
        for (const Vector2 &circle: player->_cityPositions) {
            DrawTextureV(TextureCollection::city, circle, WHITE);
        }

        // Population
        player->Update();

        EndMode2D();

        displayInfoTexts();

        EndDrawing();
    }

    // clean up everything
    TextureCollection::UnloadAll();
    delete player;

    CloseWindow();
    return 0;
}

void displayInfoTexts() {
    // instructions
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, DARKGREEN);
    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20, DARKGREEN);
    DrawText("Left-click to build a city", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80, 20, DARKGREEN);
    DrawText("Space to expand", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, DARKGREEN);
    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 160, 20, DARKGREEN);
    DrawText("Right-click to drop a bomb", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20, DARKGREEN);
    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 240, 20, DARKGREEN);

    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, DARKGREEN);

    // population
    const char *populationText = TextFormat("Population: %d / %d", player->_population, player->_maxPopulation);
    DrawText(populationText, 0 + 25, GetScreenHeight() - 50, 20, DARKGREEN);
    const char *sendText = TextFormat("People expoloring: %d", player->_peopleCurrentlyExploring);
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, DARKGREEN);
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

void checkExplosion() {
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 50;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                const int px = static_cast<int>(mousePos.x) + x;
                const int py = static_cast<int>(mousePos.y) + y;

                if (x * x + y * y <= radius * radius) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {
                        const Color explosionColor(0, 255, 0, 255);

                        ImageDrawPixel(&perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(perlinTexture);
        perlinTexture = LoadTextureFromImage(perlin);
    }
}


void initCamAndMap() {
    // Camera
    camera.target = playerPos;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // map
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
}
