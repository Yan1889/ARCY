#include <iostream>
#include <cstring>
#include <thread>
#include <vector>

#include "raylib.h"
#include "World/Loaders/TextureCollection.h"
#include "World/Player.h"
#include "World/Globals.h"
#include "World/Loaders/Sounds.h"
#include "World/Map/PerlinNoise.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650


#define MAIN_PLAYER G::players[0]
#define MAIN_PLAYER_COLOR G::players[0]._color


Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}


// ----- camera setting -----
Camera2D camera{};
constexpr int moveSpeed = 50;
constexpr int zoomSpeed = 5;
constexpr float zoomMin = 0.25f;
constexpr float zoomMax = 10.0f;

Vector2 playerPos(G::MAP_WIDTH / 2, G::MAP_HEIGHT / 2);

constexpr int botCount = 10;
constexpr int botSpawnRadius = 100;

constexpr float cityRadius = 20;


void initCamAndMap();

void initPlayers();

void gameLoop();

void handleControls();

void displayInfoTexts();

void displayAllPlayerTags();

void checkExplosion();

void checkCity();

void checkExpansionAndAttack();

Sounds mySounds; // Activate audio

int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");
    mySounds.LoadAll();

    TextureCollection::LoadAll();

    // we dont need 4000 fps - Yes we need 4000+ fps to ditch Python! Sincerely Colin
    SetTargetFPS(10000);

    initCamAndMap();
    initPlayers();

    while (!WindowShouldClose()) {
        gameLoop();
        mySounds.checkAtmosphere();
    }

    // clean up everything
    TextureCollection::UnloadAll();
    UnloadImage(G::territoryImage);

    CloseWindow();
    return 0;
}

void gameLoop() {
    handleControls();
    checkExplosion();
    checkCity();
    checkExpansionAndAttack();

    for (Player &p: G::players) {
        p.Update();
    }

    BeginDrawing();
    ClearBackground(Color{90, 90, 255, 255});

    BeginMode2D(camera);

    // terrain bg texture
    DrawTextureV(G::perlinTexture, Vector2{0, 0}, WHITE);

    for (const Player &p: G::players) {
        for (Pixel *pixel: p._borderPixels) {
            // DrawPixel(pixel->x, pixel->y, p._color);
        }
    }

    // display every city for each player
    for (const Player &p: G::players) {
        for (Pixel *cityPos: p._cityPositions) {
            DrawTextureEx(
                TextureCollection::city,
                Vector2(cityPos->x - cityRadius, cityPos->y - cityRadius),
                0,
                2 * cityRadius / TextureCollection::city.width,
                WHITE
            );
        }
    }
    // territory texture
    DrawTexture(G::territoryTexture, 0, 0, Fade(WHITE, 0.5));

    // """""Crosshair""""
    int size = 6;
    int thickness = 1;
    DrawRectangle((int) playerPos.x - size, (int) playerPos.y - thickness / 2, size * 2 + 1, thickness, WHITE);
    DrawRectangle((int) playerPos.x - thickness / 2, (int) playerPos.y - size, thickness, size * 2 + 1, WHITE);

    displayAllPlayerTags();

    EndMode2D();

    displayInfoTexts();

    EndDrawing();
}

void displayInfoTexts() {
    // instructions
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, MAIN_PLAYER_COLOR);
    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Right-click to build a city ($10K * city count)", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80,
             20, MAIN_PLAYER_COLOR);
    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, MAIN_PLAYER_COLOR);
    DrawText("1 to drop a atom bomb ($10K), 2 a hydrogen bomb ($100K)", GetScreenWidth() / 20 - 25,
             GetScreenHeight() / 20 + 160, 20, MAIN_PLAYER_COLOR);
    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Space to expand your territory", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 240, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Left-Click to attack another player", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 280, 20,
             MAIN_PLAYER_COLOR);

    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, MAIN_PLAYER_COLOR);

    // population
    float populationDisplay = static_cast<float>(MAIN_PLAYER._population);
    float maxPopulationDisplay = static_cast<float>(MAIN_PLAYER._maxPopulation);
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

    const char *sendText = TextFormat("People exploring neutral land: %d",
                                      MAIN_PLAYER._allOnGoingAttackQueues[0].second.size());
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, MAIN_PLAYER_COLOR);


    // money
    float moneyBalanceDisplay = static_cast<float>(MAIN_PLAYER._money.moneyBalance);
    const char *moneyText;

    if (moneyBalanceDisplay >= 1000000) {
        moneyBalanceDisplay /= 1000000;

        moneyText = TextFormat("Money Balance: $%.2fM", moneyBalanceDisplay);
    } else if (moneyBalanceDisplay >= 1000) {
        moneyBalanceDisplay /= 1000;

        moneyText = TextFormat("Money Balance: $%.2fK", moneyBalanceDisplay);
    } else {
        moneyText = TextFormat("Money Balance: $%.0f", moneyBalanceDisplay);
    }

    DrawText(moneyText, 25, GetScreenHeight() - 75, 20, MAIN_PLAYER_COLOR);

    // _pixelsOccupied
    const char *territorySizeText = ("pixels occupied (your size): " + std::to_string(MAIN_PLAYER._allPixels.size())).
            c_str();
    DrawText(territorySizeText, 0 + 25, GetScreenHeight() - 100, 20, MAIN_PLAYER_COLOR);
}

void displayAllPlayerTags() {
    for (int i = 0; i < G::players.size(); i++) {
        const float diameter = 2 * std::sqrt(G::players[i]._allPixels.size() / PI); // A = π * r^2 => r = sqrt(A / π)
        // Shows who you are
        const char *name = i == 0 ? "You" : ("NPC " + std::to_string(i)).c_str();
        const int charCount = strlen(name);
        const float pxWidthPerChar = diameter / charCount;

        const int fontSize = pxWidthPerChar;
        const int spacing = 1;


        Vector2 textSize = MeasureTextEx(GetFontDefault(), name, fontSize, 1);
        Vector2 textPos = {
            G::players[i]._centerPixel_x - textSize.x / 2,
            G::players[i]._centerPixel_y - textSize.y / 2
        };
        DrawTextEx(GetFontDefault(), name, textPos, fontSize, spacing, WHITE);
    }
}

void handleControls() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();

    if (playerPos.x > G::MAP_WIDTH) playerPos.x = G::MAP_WIDTH;
    else if (playerPos.x < 0) playerPos.x = 0;

    if (playerPos.y > G::MAP_HEIGHT) playerPos.y = G::MAP_HEIGHT;
    else if (playerPos.y < 0) playerPos.y = 0;

    camera.target = playerPos;

    if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed * GetFrameTime();

    if (camera.zoom < zoomMin) camera.zoom = zoomMin;
    if (camera.zoom > zoomMax) camera.zoom = zoomMax;
}

void checkExpansionAndAttack() {
    // expand with space is clicked
    if (IsKeyPressed(KEY_SPACE) && MAIN_PLAYER._population / 2 >= 100) {
        for (Player &p: G::players) {
            p.Expand(-1, 0.5);
        }
    }

    // attack player if left-click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Pixel *pixelClicked = &G::territoryMap
                [static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).x)]
                [static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).y)];
        const int playerIdClickd = pixelClicked->playerId;
        if (playerIdClickd == 0) return; // clicked on himself

        MAIN_PLAYER.Expand(playerIdClickd, 0.5);
    }


    // for testing
    if (IsKeyDown(KEY_A)) {
        for (int i = 1; i < G::players.size(); i++) {
            G::players[i].Expand(0, 0.5);
        }
    }
}

void checkExplosion() {
    if (IsKeyPressed(KEY_ONE)) {
        int cost = 10000;
        if (MAIN_PLAYER._money.moneyBalance - cost < 0) return;
        MAIN_PLAYER._money.spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 50;

        // Satz des Pythagoras kein Problem

        int a = playerPos.y - mousePos.y;
        int b = playerPos.x - mousePos.x;

        int c = sqrt(a * a + b * b);

        if (c >= 1000) {
            mySounds.Play(mySounds.distantExplosionSound);
        } else if (c >= 500) {
            mySounds.Play(mySounds.farExplosionSound);
        } else {
            mySounds.Play(mySounds.explosionSound);
        }

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int) mousePos.x + x;
                int py = (int) mousePos.y + y;

                float distance = sqrtf((float) (x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < G::MAP_WIDTH && py < G::MAP_HEIGHT) {
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
        if (MAIN_PLAYER._money.moneyBalance - cost < 0) return;
        MAIN_PLAYER._money.spendMoney(cost);

        const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        const int radius = 300;

        // Satz des Pythagoras kein Problem

        int a = playerPos.y - mousePos.y;
        int b = playerPos.x - mousePos.x;

        int c = sqrt(a * a + b * b);

        if (c >= 1000) {
            mySounds.Play(mySounds.farExplosionSound);
        } else {
            mySounds.Play(mySounds.explosionSound);
        }

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int px = (int) mousePos.x + x;
                int py = (int) mousePos.y + y;

                float distance = sqrtf((float) (x * x + y * y)) / radius;
                float noise = GetRandomValue(50, 100) / 100.0f;

                if (distance <= 1.0f && noise > distance) {
                    if (px >= 0 && py >= 0 && px < G::MAP_WIDTH && py < G::MAP_HEIGHT) {
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

void checkCity() {
    // Create cities when right-clicking
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Pixel *pixelClicked = &G::territoryMap
                [static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).x)]
                [static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).y)];

        if (MAIN_PLAYER._allPixels.contains(pixelClicked)) {
            int cost = 10000 * (MAIN_PLAYER._cityPositions.size() + 1);

            if (MAIN_PLAYER._money.moneyBalance - cost >= 0) {
                MAIN_PLAYER._money.spendMoney(cost);
                mySounds.Play(mySounds.cityBuildSound);
                MAIN_PLAYER.AddCity(GetScreenToWorld2D(GetMousePosition(), camera));
            }

            // bots also make a random city when main player makes city
            for (int i = 1; i < G::players.size(); i++) {
                cost = 10000 * (G::players[i]._cityPositions.size() + 1);
                if (G::players[i]._money.moneyBalance - cost >= 0) {
                    auto iter = G::players[i]._allPixels.begin();
                    std::advance(iter, rand() % G::players[i]._allPixels.size());
                    G::players[i]._money.spendMoney(cost);
                    G::players[i].AddCity(*iter);
                }
            }
        }
    }
}

void initPlayers() {
    // main character
    G::players.emplace_back(Player(
        &G::territoryMap[static_cast<int>(playerPos.x)][static_cast<int>(playerPos.y)],
        10
    ));

    // bots
    for (int i = 0; i < botCount; i++) {
        const float angle = 2 * PI * i / botCount;
        G::players.emplace_back(
            &G::territoryMap
            [playerPos.x + std::cos(angle) * botSpawnRadius]
            [playerPos.y + std::sin(angle) * botSpawnRadius],
            5
        );
    }
}

void initCamAndMap() {
    // Camera
    camera.target = playerPos;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // terrain
    G::perlin = GenImagePerlinNoise(
        G::MAP_WIDTH, G::MAP_HEIGHT,
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (G::MAP_WIDTH / 2),
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (G::MAP_HEIGHT / 2),
        6
    );
    std::vector<std::vector<float> > falloff = PerlinNoise::GenerateFalloffMap(G::MAP_WIDTH, G::MAP_HEIGHT);
    PerlinNoise::ApplyFalloffToImage(&G::perlin, falloff); // finally use falloff
    PerlinNoise::proceedMap(&G::perlin, G::mapParts);
    G::perlinTexture = LoadTextureFromImage(G::perlin);


    G::territoryImage = GenImageColor(G::MAP_WIDTH, G::MAP_HEIGHT, BLANK);
    G::territoryTexture = LoadTextureFromImage(G::territoryImage);


    G::territoryMap = std::vector<std::vector<Pixel> >(G::MAP_WIDTH, std::vector<Pixel>(G::MAP_HEIGHT));
    for (int y = 0; y < G::MAP_HEIGHT; y++) {
        for (int x = 0; x < G::MAP_WIDTH; x++) {
            G::territoryMap[x][y] = {x, y, -1};
        }
    }
    for (int y = 0; y < G::MAP_HEIGHT; y++) {
        for (int x = 0; x < G::MAP_WIDTH; x++) {
            G::territoryMap[x][y].LoadNeighbors();
        }
    }
}
