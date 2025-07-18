#include <iostream>
#include <cstring>
#include <thread>
#include <vector>

#include "raylib.h"
#include "loaders/TextureCollection.h"
#include "Player.h"
#include "Globals.h"
#include "loaders/Sounds.h"
#include "map/PerlinNoise.h"
#include "Bombs.h"
#include "display.h"

#define SCREEN_WIDTH 1366 // Default 980
#define SCREEN_HEIGHT 768 // Default 650


Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}


// ----- camera setting -----
constexpr int moveSpeed = 50;
constexpr int zoomSpeed = 5;
constexpr float zoomMin = 0.25f;
constexpr float zoomMax = 10.0f;

constexpr int botCount = 10;
constexpr int botSpawnRadius = 100;


void initCamAndMap();

void initPlayers();

void frameLogic();

void handleControls();

void checkExplosion();

void checkBuilding();

void checkExpansionAndAttack();

Pixel *GetPixelOnMouse();

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

    playerPos = Vector2{G::MAP_WIDTH / 2, G::MAP_HEIGHT / 2};

    while (!WindowShouldClose()) {
        frameLogic();
        displayGame();
        mySounds.checkAtmosphere();
        MAIN_PLAYER._money.getMoney(100000);
    }

    // clean up everything
    TextureCollection::UnloadAll();
    UnloadImage(G::territoryImage);

    CloseWindow();
    return 0;
}

void frameLogic() {
    handleControls();
    checkBuilding();
    checkExpansionAndAttack();
    Bombs::Update();

    for (auto & p : G::players) {
        if (!p._dead) {
            p.Update();
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
    if (IsKeyPressed(KEY_SPACE)) {
        for (auto & player : G::players) {
            if (player._dead) continue;
            player.Expand(-1, 0.5);
        }
    }

    // attack player if left-click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        const int playerIdClickd = GetPixelOnMouse()->playerId;
        if (playerIdClickd == 0) return; // clicked on himself

        MAIN_PLAYER.Expand(playerIdClickd, 0.5);
    }


    // for testing: every bot attacks you
    if (IsKeyDown(KEY_ENTER)) {
        for (int i = 1; i < G::players.size(); i++) {
            G::players[i].Expand(0, 0.5);
        }
    }
}

void checkBuilding() {
    if (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) return;
    if (!MAIN_PLAYER._allPixels.contains(GetPixelOnMouse())) return;


    // 50% chance city; 50% chance silo
    if (rand() < RAND_MAX / 2) {
        // city
        int cost = 10000 * (MAIN_PLAYER._cities.size() + 1);

        if (MAIN_PLAYER._money.moneyBalance < cost) return;
        MAIN_PLAYER._money.spendMoney(cost);
        MAIN_PLAYER.AddCity(GetPixelOnMouse());
        mySounds.Play(mySounds.cityBuildPool);

        // bots also make a random city when main player makes city
        for (int i = 1; i < G::players.size(); i++) {
            if (G::players[i]._dead) continue;

            cost = 10000 * (G::players[i]._cities.size() + 1);
            if (G::players[i]._money.moneyBalance - cost >= 0) {
                auto iter = G::players[i]._allPixels.begin();
                std::advance(iter, rand() % G::players[i]._allPixels.size());
                G::players[i]._money.spendMoney(cost);
                G::players[i].AddCity(*iter);
            }
        }
    } else {
        // silo
        int cost = 10000 * (MAIN_PLAYER._silos.size() + 1);

        if (MAIN_PLAYER._money.moneyBalance < cost) return;
        MAIN_PLAYER._money.spendMoney(cost);
        MAIN_PLAYER.AddSilo(GetPixelOnMouse());
        mySounds.Play(mySounds.cityBuildPool);

        // bots also make a random city when main player makes city
        for (int i = 1; i < G::players.size(); i++) {
            if (G::players[i]._dead) continue;

            cost = 10000 * (G::players[i]._silos.size() + 1);
            if (G::players[i]._money.moneyBalance - cost >= 0) {
                auto iter = G::players[i]._allPixels.begin();
                std::advance(iter, rand() % G::players[i]._allPixels.size());
                G::players[i]._money.spendMoney(cost);
                G::players[i].AddSilo(*iter);
            }
        }
    }
}



void initPlayers() {
    // main character
    G::players.emplace_back(
        G::PixelAt(playerPos.x, playerPos.y),
        10
    );

    // bots
    for (int i = 0; i < botCount; i++) {
        const float angle = 2 * PI * i / botCount;
        G::players.emplace_back(
            G::PixelAt(
                playerPos.x + std::cos(angle) * botSpawnRadius,
                playerPos.y + std::sin(angle) * botSpawnRadius
            ),
            5
        );
    }
}

void initCamAndMap() {
    // Camera
    camera.target = playerPos;
    camera.offset = Vector2{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
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

    G::explosionImage = GenImageColor(G::MAP_WIDTH, G::MAP_HEIGHT, BLANK);
    G::explosionTexture = LoadTextureFromImage(G::explosionImage);

    G::territoryImage = GenImageColor(G::MAP_WIDTH, G::MAP_HEIGHT, BLANK);
    G::territoryTexture = LoadTextureFromImage(G::territoryImage);
    G::territoryMap = std::vector<std::vector<Pixel> >(G::MAP_WIDTH, std::vector<Pixel>(G::MAP_HEIGHT));
    for (int y = 0; y < G::MAP_HEIGHT; y++) {
        for (int x = 0; x < G::MAP_WIDTH; x++) {
            G::territoryMap[x][y] = Pixel(x, y, -1);
        }
    }
    for (int y = 0; y < G::MAP_HEIGHT; y++) {
        for (int x = 0; x < G::MAP_WIDTH; x++) {
            G::territoryMap[x][y].LoadNeighbors();
        }
    }
}

Pixel *GetPixelOnMouse() {
    return G::PixelAt(
        static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).x),
        static_cast<int>(GetScreenToWorld2D(GetMousePosition(), camera).y)
    );
}
