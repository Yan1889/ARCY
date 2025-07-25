//
// Created by minhp on 11.07.2025.
//

#include "Sounds.h"

#include <iostream>
#include <ostream>

#include "TextureCollection.h"
#include "../Globals.h"
#include "../display/DayNightCycle.h"

using namespace G;

Sounds::Sounds()
{
    InitAudioDevice();
}

Sounds::~Sounds()
{
    delete explosionPool;
    delete cityBuildPool;
    delete misslePool;
    delete nearExplosionPool;
    delete distantExplosionPool;
    delete attackPool;

    UnloadMusicStream(oceanSound);
    UnloadMusicStream(beachSound);
    UnloadMusicStream(mountainSound);
    UnloadMusicStream(forestSound);
    UnloadMusicStream(fieldSound);
    UnloadMusicStream(radiationSound);
    UnloadMusicStream(nightAmbienceSound);
    CloseAudioDevice();
}

void Sounds::Play(SoundPool* pool)
{
    pool->Play();
}

void Sounds::LoadAll()
{
    explosionPool = new SoundPool("assets/sounds/nukeexplosion.mp3");
    nearExplosionPool = new SoundPool("assets/sounds/nearnukeexplosion.mp3");
    distantExplosionPool = new SoundPool("assets/sounds/distantnukeexplosion.mp3");
    cityBuildPool = new SoundPool("assets/sounds/citybuilding.mp3");
    misslePool = new SoundPool("assets/sounds/nukeflyby.mp3");
    attackPool = new SoundPool("assets/sounds/attack.mp3");

    oceanSound = LoadMusicStream("assets/sounds/ocean.mp3");
    beachSound = LoadMusicStream("assets/sounds/beach.mp3");
    mountainSound = LoadMusicStream("assets/sounds/mountain.mp3");
    forestSound = LoadMusicStream("assets/sounds/forest.mp3");
    fieldSound = LoadMusicStream("assets/sounds/field.mp3");
    radiationSound = LoadMusicStream("assets/sounds/radiation.mp3");
    nightAmbienceSound = LoadMusicStream("assets/sounds/nightambience.mp3");

    if (nightAmbienceSound.frameCount == 0) std::cerr << "Sound not found!!!!!" << std::endl;

    PlayMusicStream(oceanSound);
    PlayMusicStream(beachSound);
    PlayMusicStream(mountainSound);
    PlayMusicStream(fieldSound);
    PlayMusicStream(forestSound);
    PlayMusicStream(radiationSound);
    PlayMusicStream(nightAmbienceSound);
}

void Sounds::checkAtmosphere()
{
    UpdateMusicStream(oceanSound);
    UpdateMusicStream(beachSound);
    UpdateMusicStream(mountainSound);
    UpdateMusicStream(fieldSound);
    UpdateMusicStream(forestSound);
    UpdateMusicStream(radiationSound);
    UpdateMusicStream(nightAmbienceSound);

    const Color pixelColor = GetImageColor(G::perlin, static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));
    const bool isContaminated = G::territoryMap[static_cast<int>(playerPos.x)][static_cast<int>(playerPos.y)].contaminated;

    std::vector<Color> mapColors = {
        Color{90, 90, 255, 255},
        Color{125, 125, 255, 255},
        Color{247, 252, 204, 255},

        Color{129, 245, 109, 255},
        Color{117, 219, 99, 255},
        Color{97, 184, 81, 255},
        Color{191, 191, 191, 255},
        Color{153, 153, 153, 255},
        Color{255, 255, 255, 255}
    };

    // Contaminated zone
    if (isContaminated)
    {
        ResumeMusicStream(radiationSound);
        PauseMusicStream(oceanSound);
        PauseMusicStream(beachSound);
        PauseMusicStream(fieldSound);
        PauseMusicStream(forestSound);
        PauseMusicStream(mountainSound);
    }
    else PauseMusicStream(radiationSound);

    if (isContaminated) return;

    // Ocean
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[0]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[1])) ResumeMusicStream(oceanSound);
    else if (playerPos.x < 0 || playerPos.x >= MAP_WIDTH || playerPos.y < 0 || playerPos.y >= MAP_HEIGHT) ResumeMusicStream(oceanSound);
    else PauseMusicStream(oceanSound);

    // Beach
    if (!(DayNightCycle::time > 0.25f && DayNightCycle::time < 0.75f) &&
        ColorToInt(pixelColor) == ColorToInt(mapColors[2])) ResumeMusicStream(beachSound);
    else PauseMusicStream(beachSound);

    // Field
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[3]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[4]))
    {
        if (!(DayNightCycle::time > 0.25f && DayNightCycle::time < 0.75f))
        {
            ResumeMusicStream(fieldSound);
        }
        else PauseMusicStream(fieldSound);
    }
    else PauseMusicStream(fieldSound);

    // Forest
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[5]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[6]))
    {
        if (!(DayNightCycle::time > 0.25f && DayNightCycle::time < 0.75f))
        {
            ResumeMusicStream(forestSound);
        }
        else PauseMusicStream(forestSound);
    }
    else PauseMusicStream(forestSound);

    // Mountain
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[7]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[8])) ResumeMusicStream(mountainSound);
    else PauseMusicStream(mountainSound);

    // Night Ambience
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[3]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[4]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[5]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[6]))
    {
        if (DayNightCycle::time > 0.25f && DayNightCycle::time < 0.75f) ResumeMusicStream(nightAmbienceSound);
        else PauseMusicStream(nightAmbienceSound);
    }
    else PauseMusicStream(nightAmbienceSound);
}

