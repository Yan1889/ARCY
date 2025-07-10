#include <iostream>
#include <thread>
#include <vector>

#include "raylib.h"
#include "World/TextureCollection.h"
#include "World/Player.h"
#include "World/Globals.h"
#include "World/Money.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650
#define MAIN_PLAYER_COLOR players[0]._color

#define MAP_HEIGHT 2500
#define MAP_WIDTH 2500


Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}


// ----- camera setting -----
Camera2D camera{};
constexpr int moveSpeed = 100;
constexpr int zoomSpeed = 5;
constexpr float zoomMin = 0.1f;
constexpr float zoomMax = 10.0f;

Vector2 playerPos(MAP_WIDTH / 2, MAP_HEIGHT / 2);
std::vector<Player> players{};

constexpr int botCount = 10;
constexpr int botSpawnRadius = 500;


void initCamAndMap();

void handleControls();

void displayInfoTexts();

void checkExplosion();

void displayUser();

int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");
    TextureCollection::LoadAll();

    // we dont need 4000 fps - Yes we need 4000+ fps to ditch Python! Sincerely Colin
    SetTargetFPS(10000);

    initCamAndMap();

    // main character
    players.push_back(Player(
        {
            static_cast<int>(playerPos.x),
            static_cast<int>(playerPos.y)
        },
        10
    ));

    // 10 bots
    for (int i = 0; i < botCount; i++) {
        float angle = 2 * PI * i / botCount;
        players.push_back(Player(
            {
                static_cast<int>(playerPos.x + std::cos(angle) * botSpawnRadius),
                static_cast<int>(playerPos.y + std::sin(angle) * botSpawnRadius)
            },
            5
        ));
    }

    for (Player &p: players) {
        p._cooldownTime = 1.0f;
        p._lastActionTime = GetTime();
    }

    while (!WindowShouldClose())
    {
        handleControls();
        checkExplosion();

        BeginDrawing();
        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        // terrain bg texture
        DrawTextureV(G::perlinTexture, Vector2{0, 0}, WHITE);
        // territory texture
        DrawTexture(G::territoryTexture, 0, 0, Fade(WHITE, 0.5));

        // I think it's okay to draw a few border pixels manually with this loop, what do you think @Colin?
        for (const Player &p: players) {
            for (const Pixel &pixel: p._frontierPixels) {
                DrawPixel(pixel.x, pixel.y, p._color);
            }
        }
        // I think I spider @Yan!!!!!!!!!

        // display each city of main character
        for (const Vector2 &circle: players[0]._cityPositions) {
            DrawTextureV(TextureCollection::city, circle, WHITE);
        }

        // Population
        for (Player &p: players) {
            p.Update();
        }

        displayUser();

        EndMode2D();

        displayInfoTexts();

        EndDrawing();
    }

    // clean up everything
    TextureCollection::UnloadAll();

    CloseWindow();
    return 0;
}

void displayInfoTexts() {
    // instructions
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, MAIN_PLAYER_COLOR);
    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20, MAIN_PLAYER_COLOR);
    DrawText("Left-click to build a city ($10K * city count)", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80,
             20, MAIN_PLAYER_COLOR);
    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, MAIN_PLAYER_COLOR);
    DrawText("1 to drop a atom bomb ($10K), 2 a hydrogen bomb ($100K)", GetScreenWidth() / 20 - 25,
             GetScreenHeight() / 20 + 160, 20, MAIN_PLAYER_COLOR);
    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20, MAIN_PLAYER_COLOR);
    DrawText("Space to expand your territory", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 240, 20, MAIN_PLAYER_COLOR);

    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, MAIN_PLAYER_COLOR);

    // population
    float populationDisplay = static_cast<float>(players[0]._population);
    float maxPopulationDisplay = static_cast<float>(players[0]._maxPopulation);
    const char *populationText;

    if (maxPopulationDisplay >= 1000) {
        maxPopulationDisplay /= 1000;

        if (populationDisplay >= 1000) {
            populationDisplay /= 1000;

            populationText = TextFormat("Population: %.2fK / %.2fK", populationDisplay, maxPopulationDisplay);
        } else {
            populationText = TextFormat("Population: %.0f / %.2fK", populationDisplay, maxPopulationDisplay);
        }
    }

    DrawText(populationText, 0 + 25, GetScreenHeight() - 50, 20, MAIN_PLAYER_COLOR);
    const char *sendText = TextFormat("People exploring: %d", players[0]._peopleCurrentlyExploring);
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, MAIN_PLAYER_COLOR);

    // money
    float moneyBalanceDisplay = static_cast<float>(players[0]._money.moneyBalance);
    const char *moneyText;

    if (moneyBalanceDisplay >= 1000000) {
        moneyBalanceDisplay /= 1000000;

        moneyText = TextFormat("Money Balance: $%.2fM", moneyBalanceDisplay);
    }
    else if (moneyBalanceDisplay >= 1000) {
        moneyBalanceDisplay /= 1000;

        moneyText = TextFormat("Money Balance: $%.2fK", moneyBalanceDisplay);
    } else {
        moneyText = TextFormat("Money Balance: $%.0f", moneyBalanceDisplay);
    }

    DrawText(moneyText, 25, GetScreenHeight() - 75, 20, MAIN_PLAYER_COLOR);

    // _pixelsOccupied
    const char* territorySizeText = ("pixels occupied (your size): " + std::to_string(players[0]._allPixels.size())).c_str();
    DrawText(territorySizeText, 0 + 25, GetScreenHeight() - 100, 20, MAIN_PLAYER_COLOR);
}

void displayUser()
{
    // Shows who you are
    const char* yourName = "You";
    int fontSize = 10;
    int spacing = 1;

    Vector2 userTextPos = {MAP_WIDTH / 2, MAP_HEIGHT / 2};

    Vector2 textSize = MeasureTextEx(GetFontDefault(), yourName, fontSize, 1);
    Vector2 textPos = {
        userTextPos.x - textSize.x / 2,
        userTextPos.y - textSize.y / 2

    };

    DrawTextEx(GetFontDefault(), yourName, textPos, fontSize, spacing, WHITE);
}

void handleControls() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();

    // expand with space is clicked
    if (IsKeyPressed(KEY_SPACE) && players[0]._population / 2 >= 100) {
        for (Player& p : players) {
           p.Expand(0.5);
        }
    }
    // Create cities when left-clicking
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (Player& p : players) {
            const int cost = 10000 * (p._cityPositions.size() + 1);
            if (p._money.moneyBalance - cost >= 0) {
                p._money.spendMoney(cost);
                p.AddCity(GetScreenToWorld2D(GetMousePosition(), camera));
            }

        }
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
        if (players[0]._money.moneyBalance - cost < 0) return;
        players[0]._money.spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 50;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int) mousePos.x + x;
                int py = (int) mousePos.y + y;

                float distance = sqrtf((float) (x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {
                        Color explosionColor = Color{
                            (unsigned char) GetRandomValue(0, 200), // Red
                            (unsigned char) GetRandomValue(230, 255), // Green
                            (unsigned char) GetRandomValue(0, 50), // Blue                                       // Blue
                            255 // Alpha
                        };

                        ImageDrawPixel(&G::perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(G::perlinTexture);
        G::perlinTexture = LoadTextureFromImage(G::perlin);
    } else if (IsKeyPressed(KEY_TWO)) {
        int cost = 100000;
        if (players[0]._money.moneyBalance - cost < 0) return;
        players[0]._money.spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 300;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int) mousePos.x + x;
                int py = (int) mousePos.y + y;

                float distance = sqrtf((float) (x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < MAP_WIDTH && py < MAP_HEIGHT) {
                        Color explosionColor = Color{
                            (unsigned char) GetRandomValue(0, 200), // Red
                            (unsigned char) GetRandomValue(230, 255), // Green
                            (unsigned char) GetRandomValue(0, 50), // Blue                                       // Blue
                            255 // Alpha
                        };

                        ImageDrawPixel(&G::perlin, px, py, explosionColor);
                    }
                }
            }
        }

        UnloadTexture(G::perlinTexture);
        G::perlinTexture = LoadTextureFromImage(G::perlin);
    }
}

void initCamAndMap() {
    // Camera
    camera.target = playerPos;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // map
    G::InitMap(MAP_WIDTH, MAP_HEIGHT);

    // terrain
    G::perlin = GenImagePerlinNoise(
        MAP_WIDTH, MAP_HEIGHT,
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (MAP_WIDTH / 2),
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (MAP_HEIGHT / 2),
        6
    );
    std::vector<std::vector<float> > falloff = PerlinNoise::GenerateFalloffMap(MAP_WIDTH, MAP_HEIGHT);
    PerlinNoise::ApplyFalloffToImage(&G::perlin, falloff); // finally use falloff
    PerlinNoise::proceedMap(&G::perlin, G::mapParts);
    G::perlinTexture = LoadTextureFromImage(G::perlin);


    G::territoryMap = std::vector<std::vector<Pixel> >(MAP_HEIGHT, std::vector<Pixel>(MAP_WIDTH));
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            G::territoryMap[y][x] = {x, y, 0};
        }
    }
    G::territoryImage = GenImageColor(G::WIDTH, G::HEIGHT, BLANK);
    G::territoryTexture = LoadTextureFromImage(G::territoryImage);
}
