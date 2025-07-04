#include <iostream>
#include <thread>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "World/PerlinNoise.h"
#include "World/TextureCollection.h"
#include "World/Player.h"
#include "World/Globals.h"
#include "World/Money.h"

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


// ----- camera setting -----
Camera2D camera{};
const int moveSpeed = 100;
const int zoomSpeed = 5;
const float zoomMin = 0.1f;
const float zoomMax = 10.0f;


Vector2 playerPos(MAP_WIDTH / 2, MAP_HEIGHT / 2);
Player *player;

Money *money;
float cooldownTime;
float lastActionTime;

void initCamAndMap();

void handleControls();

void displayInfoTexts();

void checkExplosion();

void increaseMoney();


int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");
    TextureCollection::LoadAll();

    // we dont need 4000 fps - Yes we need 4000+ fps to ditch Python! Sincerely Colin
    SetTargetFPS(10000);

    initCamAndMap();

    player = new Player(
        {
            static_cast<int>(playerPos.x),
            static_cast<int>(playerPos.y)
        },
        10,
        perlin
    );

    money = new Money();
    cooldownTime = 1.0f;
    lastActionTime = GetTime();

    std::cout << player->_population << std::endl;

    while (!WindowShouldClose()) {
        handleControls();
        checkExplosion();

        // Add money depending on population
        increaseMoney();

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Create cities when left-clicking
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int cost = 10000 * (player->_cityPositions.size() + 1);

            if (money ->moneyBalance - cost >= 0)
            {
                money->spendMoney(cost);
                player->AddCity(GetScreenToWorld2D(GetMousePosition(), camera));
            }
        }

        BeginDrawing();
        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        for (const Pixel &pixel: player->_frontierPixels) {
            DrawPixel(pixel.x, pixel.y, RED);
        }

        /* Code noch nicht ausprobiert (neue Methode)
        
        for (const Pixel &pixel: player->_allPixels) {
            ImageDrawPixel(&perlin, pixel.x, pixel.y, Fade(ORANGE, 0.5));
        }
        
        UnloadTexture(perlinTexture);
        perlinTexture = LoadTextureFromImage(perlin);
        
        */

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
    delete money;

    CloseWindow();
    return 0;
}

void displayInfoTexts() {
    // instructions
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, DARKGREEN);
    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20, DARKGREEN);
    DrawText("Left-click to build a city ($10K * city count)", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80, 20, DARKGREEN);
    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, DARKGREEN);
    DrawText("1 to drop a atom bomb ($10K), 2 a hydrogen bomb ($100K)", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 160, 20, DARKGREEN);
    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20, DARKGREEN);
    DrawText("Space to expand your territory", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 240, 20, DARKGREEN);

    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, DARKGREEN);

    // population
    float populationDisplay = static_cast<float>(player->_population);
    float maxPopulationDisplay = static_cast<float>(player->_maxPopulation);
    const char *populationText;

    if (maxPopulationDisplay >= 1000)
    {
        maxPopulationDisplay /= 1000;

        if (populationDisplay >= 1000)
        {
            populationDisplay /= 1000;

            populationText = TextFormat("Population: %.2fK / %.2fK", populationDisplay, maxPopulationDisplay);
        }
        else
        {
            populationText = TextFormat("Population: %.0f / %.2fK", populationDisplay, maxPopulationDisplay);
        }
    }

    DrawText(populationText, 0 + 25, GetScreenHeight() - 50, 20, DARKGREEN);
    const char *sendText = TextFormat("People exploring: %d", player->_peopleCurrentlyExploring);
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, DARKGREEN);

    // money
    float moneyBalanceDisplay = static_cast<float>(money->moneyBalance);
    const char *moneyText;

    if (moneyBalanceDisplay >= 1000)
    {
        moneyBalanceDisplay /= 1000;

        moneyText = TextFormat("Money Balance: $%.2fK", moneyBalanceDisplay);
    }
    else
    {
        moneyText = TextFormat("Money Balance: $%.0fK", moneyBalanceDisplay);
    }

    DrawText(moneyText, 25, GetScreenHeight() - 75, 20, DARKGREEN);
}

void handleControls() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();

    // expand with space is clicked
    if (IsKeyPressed(KEY_SPACE) && player->_population / 2 >= 100) {
        player->Expand(0.5);
    }

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
    if (IsKeyPressed(KEY_ONE)) {
        int cost = 10000;
        if (money->moneyBalance - cost < 0) return;
        money->spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 50;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int)mousePos.x + x;
                int py = (int)mousePos.y + y;

                float distance = sqrtf((float)(x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {

                        Color explosionColor = Color{
                            (unsigned char)GetRandomValue(0, 200),  // Red
                            (unsigned char)GetRandomValue(230, 255),   // Green
                            (unsigned char)GetRandomValue(0, 50),  // Blue                                       // Blue
                            255  // Alpha
                        };

                        ImageDrawPixel(&perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(perlinTexture);
        perlinTexture = LoadTextureFromImage(perlin);
    }
    else if (IsKeyPressed(KEY_TWO)) {
        int cost = 100000;
        if (money->moneyBalance - cost < 0) return;
        money->spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 300;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int)mousePos.x + x;
                int py = (int)mousePos.y + y;

                float distance = sqrtf((float)(x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {

                        Color explosionColor = Color{
                            (unsigned char)GetRandomValue(0, 200),  // Red
                            (unsigned char)GetRandomValue(230, 255),   // Green
                            (unsigned char)GetRandomValue(0, 50),  // Blue                                       // Blue
                            255  // Alpha
                        };

                        ImageDrawPixel(&perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(perlinTexture);
        perlinTexture = LoadTextureFromImage(perlin);
    }
}

void increaseMoney()
{
    float currentTime = GetTime();

    if (currentTime - lastActionTime >= cooldownTime)
    {
        int peopleAddition = 2;
        int totalAddition = peopleAddition * player->_population;
        money->moneyBalance += totalAddition;

        lastActionTime = currentTime;
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
    PerlinNoise::proceedMap(&perlin, G::map);
    perlinTexture = LoadTextureFromImage(perlin);
}
