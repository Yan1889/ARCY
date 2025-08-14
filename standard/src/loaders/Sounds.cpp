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
using namespace Terrain;

Sounds::Sounds() {
    InitAudioDevice();
}

Sounds::~Sounds() {
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

void Sounds::Play(SoundPool *pool) {
    pool->Play();
}

void Sounds::LoadAll() {
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

void Sounds::checkAtmosphere() {
    UpdateMusicStream(oceanSound);
    UpdateMusicStream(beachSound);
    UpdateMusicStream(mountainSound);
    UpdateMusicStream(fieldSound);
    UpdateMusicStream(forestSound);
    UpdateMusicStream(radiationSound);
    UpdateMusicStream(nightAmbienceSound);

    Pixel *pixel = G::PixelAt(playerPos);
    const Kind kind = GetKindAt(pixel);

    // Contaminated zone
    if (pixel->contaminated) {
        ResumeMusicStream(radiationSound);
        PauseMusicStream(oceanSound);
        PauseMusicStream(beachSound);
        PauseMusicStream(fieldSound);
        PauseMusicStream(forestSound);
        PauseMusicStream(mountainSound);
        return;
    }
    PauseMusicStream(radiationSound);

    // Ocean
    if (kind == DEEP_WATER || kind == LOW_WATER
        || playerPos.x < 0 || playerPos.x >= MAP_WIDTH || playerPos.y < 0 || playerPos.y >= MAP_HEIGHT)
        ResumeMusicStream(oceanSound);
    else PauseMusicStream(oceanSound);

    // Beach
    if (!DayNightCycle::isNightTime && kind == BEACH)
        ResumeMusicStream(beachSound);
    else PauseMusicStream(beachSound);

    // Field
    if (kind == OPEN_FIELD || kind == HILLS) {
        if (!DayNightCycle::isNightTime) {
            ResumeMusicStream(fieldSound);
        } else PauseMusicStream(fieldSound);
    } else PauseMusicStream(fieldSound);

    // Forest
    if (kind == FORREST || kind == STONE && !DayNightCycle::isNightTime) {
        ResumeMusicStream(forestSound);
    } else PauseMusicStream(forestSound);

    // Mountain
    if (kind == MOUNTAIN || kind == SNOW)
        ResumeMusicStream(mountainSound);
    else PauseMusicStream(mountainSound);

    // Night Ambience
    if (kind == BEACH || kind == OPEN_FIELD || kind == HILLS || kind == FORREST && DayNightCycle::isNightTime) {
        ResumeMusicStream(nightAmbienceSound);
    } else PauseMusicStream(nightAmbienceSound);
}
