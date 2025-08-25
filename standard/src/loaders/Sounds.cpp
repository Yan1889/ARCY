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

    const Pixel *p = G::PixelAt(playerPos);
    const auto k_eq = [p](const Kind k) { return k == p->kind; };

    // Contaminated zone
    if (p->contaminated) {
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
    if (k_eq(DEEP_WATER) || k_eq(LOW_WATER)
        || playerPos.x < 0 || playerPos.x >= MAP_WIDTH || playerPos.y < 0 || playerPos.y >= MAP_HEIGHT)
        ResumeMusicStream(oceanSound);
    else PauseMusicStream(oceanSound);

    // Beach
    if (!DayNightCycle::isNightTime && k_eq(BEACH))
        ResumeMusicStream(beachSound);
    else PauseMusicStream(beachSound);

    // Field
    if (k_eq(OPEN_FIELD) || k_eq(HILLS)) {
        if (!DayNightCycle::isNightTime) {
            ResumeMusicStream(fieldSound);
        } else PauseMusicStream(fieldSound);
    } else PauseMusicStream(fieldSound);

    // Forest
    if ((k_eq(FORREST) || k_eq(STONE)) && !DayNightCycle::isNightTime) {
        ResumeMusicStream(forestSound);
    } else PauseMusicStream(forestSound);

    // Mountain
    if (k_eq(MOUNTAIN) || k_eq(SNOW))
        ResumeMusicStream(mountainSound);
    else PauseMusicStream(mountainSound);

    // Night Ambience
    if ((k_eq(BEACH) || k_eq(OPEN_FIELD) || k_eq(HILLS) || k_eq(FORREST)) && DayNightCycle::isNightTime) {
        ResumeMusicStream(nightAmbienceSound);
    } else PauseMusicStream(nightAmbienceSound);
}
