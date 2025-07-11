//
// Created by minhp on 11.07.2025.
//

#include "Sounds.h"

#include <iostream>
#include <ostream>

#include "TextureCollection.h"
#include "Globals.h"
#include "PerlinNoise.h"

Sounds::Sounds()
{
    InitAudioDevice();
}

Sounds::~Sounds()
{
    UnloadSound(explosionSound);

    UnloadMusicStream(oceanSound);
    UnloadMusicStream(beachSound);
    UnloadMusicStream(mountainSound);
    UnloadMusicStream(forestSound);
    UnloadMusicStream(fieldSound);
    CloseAudioDevice();
}

void Sounds::LoadAll()
{
    explosionSound = LoadSound("assets/sounds/nukeexplosion.mp3");
    oceanSound = LoadMusicStream("assets/sounds/ocean.mp3");
    beachSound = LoadMusicStream("assets/sounds/beach.mp3");
    mountainSound = LoadMusicStream("assets/sounds/mountain.mp3");
    forestSound = LoadMusicStream("assets/sounds/forest.mp3");
    fieldSound = LoadMusicStream("assets/sounds/field.mp3");

    PlayMusicStream(oceanSound);
    PlayMusicStream(beachSound);
    PlayMusicStream(mountainSound);
    PlayMusicStream(fieldSound);
    PlayMusicStream(forestSound);
}

void Sounds::Play(Sound sound)
{
    PlaySound(sound);
}

void Sounds::checkAtmosphere()
{
    UpdateMusicStream(oceanSound);
    UpdateMusicStream(beachSound);
    UpdateMusicStream(mountainSound);
    UpdateMusicStream(fieldSound);
    UpdateMusicStream(forestSound);

    Color pixelColor = GetImageColor(G::perlin, static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));

    // Ocean
    if (ColorToInt(pixelColor) == ColorToInt(Color{90, 90, 255, 255}) ||
        ColorToInt(pixelColor) == ColorToInt(Color{125, 125, 255, 255})) ResumeMusicStream(oceanSound);
    else if (playerPos.x < 0 || playerPos.x > mapWidth || playerPos.y < 0 || playerPos.y > mapHeight) ResumeMusicStream(oceanSound);
    else PauseMusicStream(oceanSound);

    // Beach
    if (ColorToInt(pixelColor) == ColorToInt(Color{247, 252, 204, 255})) ResumeMusicStream(beachSound);
    else PauseMusicStream(beachSound);

    // Field
    if (ColorToInt(pixelColor) == ColorToInt(Color{129, 245, 109, 255}) ||
        ColorToInt(pixelColor) == ColorToInt(Color{117, 219, 99, 255})) ResumeMusicStream(fieldSound);
    else PauseMusicStream(fieldSound);

    // Forest
    if (ColorToInt(pixelColor) == ColorToInt(Color{97, 184, 81, 255}) ||
        ColorToInt(pixelColor) == ColorToInt(Color{191, 191, 191, 255})) ResumeMusicStream(forestSound);
    else PauseMusicStream(forestSound);

    // Mountain
    if (ColorToInt(pixelColor) == ColorToInt(Color{153, 153, 153, 255}) ||
        ColorToInt(pixelColor) == ColorToInt(Color{255, 255, 255, 255})) ResumeMusicStream(mountainSound);
    else PauseMusicStream(mountainSound);
}

