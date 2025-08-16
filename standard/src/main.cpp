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
#include  "map/ChunkGeneration.h"

using namespace G;


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
    ChunkGeneration::InitChunkGeneration(16, 16);
    //ChunkGeneration::InitFalloff(); just testing without falloff

    // don't remove: triggering the chunk generation before placing the players
    ChunkGeneration::GetVisibleChunks(camera);
    initPlayers();

    while (!WindowShouldClose()) {
        if (!gameOver) {
            frameLogic();
        }
        displayGame();

        mySounds.checkAtmosphere();
        MAIN_PLAYER._money.getMoney(100000); //only for testing purposes!
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
    if (IsKeyDown(KEY_W) && !buildMenuShown) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_S) && !buildMenuShown) playerPos.y += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_A) && !buildMenuShown) playerPos.x -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyDown(KEY_D) && !buildMenuShown) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (IsKeyDown(KEY_ESCAPE)) WindowShouldClose();
    if (IsKeyPressed(KEY_Q)) showLeaderboard = !showLeaderboard;
    if (IsKeyPressed(KEY_E)) showControls = !showControls;

    // Building shortcuts
    if (IsKeyPressed(KEY_ONE) && buildMenuShown) MAIN_PLAYER.TryAddCity(GetPixelOnMouse());
    if (IsKeyPressed(KEY_TWO) && buildMenuShown) MAIN_PLAYER.TryAddSilo(GetPixelOnMouse());
    if (IsKeyPressed(KEY_THREE) && buildMenuShown) MAIN_PLAYER.TryLaunchAtomBomb(GetPixelOnMouse());
    if (IsKeyPressed(KEY_FOUR) && buildMenuShown) MAIN_PLAYER.TryLaunchHydrogenBomb(GetPixelOnMouse());

    int offset = 10;
    if (playerPos.x > MAP_WIDTH - offset) playerPos.x = MAP_WIDTH - offset;
    else if (playerPos.x < 0) playerPos.x = 0;

    if (playerPos.y > MAP_HEIGHT - offset) playerPos.y = MAP_HEIGHT - offset;
    else if (playerPos.y < 0) playerPos.y = 0;

    camera.target = playerPos;

    if (IsKeyDown(KEY_UP) && !buildMenuShown) camera.zoom += zoomSpeed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN) && !buildMenuShown) camera.zoom -= zoomSpeed * GetFrameTime();

    if (camera.zoom < zoomMin) camera.zoom = zoomMin;
    if (camera.zoom > zoomMax) camera.zoom = zoomMax;
}

void checkExpansionAndAttack() {
    // attack player if left-click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !buildMenuShown) {
        const int playerIdClickd = GetPixelOnMouse() == nullptr ? -1 : GetPixelOnMouse()->playerId;
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
    Pixel *mainCharacterSpawnPixel = Terrain::FindRandomPixelWithKind(Terrain::BEACH);
    playerPos = mainCharacterSpawnPixel->ToVector2();
    players.emplace_back(
        mainCharacterSpawnPixel,
        std::string("You")
    );

    // bots
    for (int i = 0; i < botCount; i++) {
        players.emplace_back(
            Terrain::FindRandomPixelWithKind(Terrain::BEACH),
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

    explosionImage = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLANK);
    explosionTexture = LoadTextureFromImage(explosionImage);

    territoryImage = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLANK);
    territoryTexture = LoadTextureFromImage(territoryImage);
    territoryMap = std::vector<Pixel>(MAP_WIDTH * MAP_HEIGHT);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            territoryMap[G::ToIdx(x, y)] = Pixel(x, y);
        }
    }
    for (Pixel &p: territoryMap) {
        p.LoadNeighbors();
    }
    playerPos = Vector2{static_cast<float>(MAP_WIDTH) / 2, static_cast<float>(MAP_HEIGHT) / 2};
    initDisplay();
}
