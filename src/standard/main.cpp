#include <cmath>
#include <iostream>
#include <cstring>
#include <thread>
#include <vector>

#include "raylib.h"
#include "loaders/TextureCollection.h"
#include "player/Player.h"
#include "Globals.h"
#include "loaders/Sounds.h"
#include "map/PerlinNoise.h"
#include "Bombs.h"
#include "display/display.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650

using namespace G;

Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}


// ----- camera setting -----
constexpr int moveSpeed = 50;
constexpr int zoomSpeed = 5;
constexpr float zoomMin = 0.25f;
constexpr float zoomMax = 10.0f;

constexpr int botCount = 20;
constexpr int botSpawnRadius = 500;


void initCamAndMap();

void initPlayers();

void frameLogic();

void handleControls();

void checkExplosion();

void checkGameOver();

void checkExpansionAndAttack();


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
        if (!gameOver) {
            frameLogic();
        }
        displayGame();
        mySounds.checkAtmosphere();
        MAIN_PLAYER._money.getMoney(100000);
    }

    // clean up everything
    TextureCollection::UnloadAll();
    UnloadImage(territoryImage);

    CloseWindow();
    return 0;
}

void frameLogic() {
    handleControls();
    checkExpansionAndAttack();
    checkGameOver();
    Bombs::Update();

    players[0].Update();
    for (int i = 1; i < players.size(); i++) {
        Player &bot = players[i];
        if (!bot._dead) {
            bot.BotLogic();
            bot.Update();
        }
    }
}

void handleControls() {
    if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();

    if (playerPos.x > MAP_WIDTH) playerPos.x = MAP_WIDTH;
    else if (playerPos.x < 0) playerPos.x = 0;

    if (playerPos.y > MAP_HEIGHT) playerPos.y = MAP_HEIGHT;
    else if (playerPos.y < 0) playerPos.y = 0;

    camera.target = playerPos;

    if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed * GetFrameTime();

    if (camera.zoom < zoomMin) camera.zoom = zoomMin;
    if (camera.zoom > zoomMax) camera.zoom = zoomMax;
}

void checkExpansionAndAttack() {
    // attack player if left-click
    const bool mouseOverBuildMenu = buildMenuShown && GetMousePosition().y > GetScreenHeight() * 0.9;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !mouseOverBuildMenu) {
        const int playerIdClickd = GetPixelOnMouse()->playerId;
        MAIN_PLAYER.Expand(playerIdClickd, 0.5);
    }
}

void checkGameOver() {
    int playersAlive{};
    for (const Player &p: players) {
        if (p._dead) continue;

        playersAlive++;
        if (playersAlive >= 2) {
            // 2 or more players alive => game still going on
            return;
        }
    }
    gameOver = true;

    // set winner
    for (int i = 0; i < players.size(); i++) {
        if (players[i]._dead) continue;

        winnerId = i;
        return;
    }
}

void initPlayers() {
    // main character
    players.emplace_back(
        PixelAt(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y)),
        std::string("You")
    );

    // bots
    for (int i = 0; i < botCount; i++) {
        const float angle = 2 * PI * static_cast<float>(i) / botCount;
        players.emplace_back(
            PixelAt(
                static_cast<int>(playerPos.x + std::cos(angle) * botSpawnRadius),
                static_cast<int>(playerPos.y + std::sin(angle) * botSpawnRadius)
            ),
            std::string("NPC ") + std::to_string(i)
        );
        players.back()._bot = true;
    }
}

void initCamAndMap() {
    // Camera
    camera.target = playerPos;
    camera.offset = Vector2{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // terrain
    perlin = GenImagePerlinNoise(
        MAP_WIDTH, MAP_HEIGHT,
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (MAP_WIDTH / 2),
        static_cast<int>(rand() * 10000.0f / RAND_MAX) * (MAP_HEIGHT / 2),
        6
    );
    std::vector<std::vector<float> > falloff = PerlinNoise::GenerateFalloffMap(MAP_WIDTH, MAP_HEIGHT);
    PerlinNoise::ApplyFalloffToImage(&perlin, falloff); // finally use falloff
    PerlinNoise::proceedMap(&perlin, mapParts);
    perlinTexture = LoadTextureFromImage(perlin);

    explosionImage = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLANK);
    explosionTexture = LoadTextureFromImage(explosionImage);

    territoryImage = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLANK);
    territoryTexture = LoadTextureFromImage(territoryImage);
    territoryMap = std::vector<std::vector<Pixel>>(MAP_WIDTH, std::vector<Pixel>(MAP_HEIGHT));
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            territoryMap[x][y] = Pixel(x, y, -1);
        }
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            territoryMap[x][y].LoadNeighbors();
        }
    }
    playerPos = Vector2{static_cast<float>(MAP_WIDTH) / 2, static_cast<float>(MAP_HEIGHT) / 2};
    initDisplay();
}
