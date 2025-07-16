//
// Created by minhp on 11.07.2025.
//

#include "Sounds.h"
#include "TextureCollection.h"
#include "../Globals.h"

Sounds::Sounds()
{
    InitAudioDevice();
}

Sounds::~Sounds()
{
    UnloadSound(explosionSound);
    UnloadSound(cityBuildSound);
    UnloadSound(farExplosionSound);
    UnloadSound(distantExplosionSound);
    UnloadSound(missleSound);

    UnloadMusicStream(oceanSound);
    UnloadMusicStream(beachSound);
    UnloadMusicStream(mountainSound);
    UnloadMusicStream(forestSound);
    UnloadMusicStream(fieldSound);
    UnloadMusicStream(radiationSound);
    CloseAudioDevice();
}

void Sounds::LoadAll()
{
    explosionSound = LoadSound("assets/sounds/nukeexplosion.mp3");
    cityBuildSound = LoadSound("assets/sounds/citybuilding.mp3");
    farExplosionSound = LoadSound("assets/sounds/nearnukeexplosion.mp3");
    distantExplosionSound = LoadSound("assets/sounds/distantnukeexplosion.mp3");
    missleSound = LoadSound("assets/sounds/nukeflyby.mp3");

    oceanSound = LoadMusicStream("assets/sounds/ocean.mp3");
    beachSound = LoadMusicStream("assets/sounds/beach.mp3");
    mountainSound = LoadMusicStream("assets/sounds/mountain.mp3");
    forestSound = LoadMusicStream("assets/sounds/forest.mp3");
    fieldSound = LoadMusicStream("assets/sounds/field.mp3");
    radiationSound = LoadMusicStream("assets/sounds/radiation.mp3");

    PlayMusicStream(oceanSound);
    PlayMusicStream(beachSound);
    PlayMusicStream(mountainSound);
    PlayMusicStream(fieldSound);
    PlayMusicStream(forestSound);
    PlayMusicStream(radiationSound);
}

void Sounds::Play(Sound sound)
{
    PlaySound(sound);
}

void Sounds::Stop(Sound sound)
{
    StopSound(sound);
}

void Sounds::checkAtmosphere()
{
    UpdateMusicStream(oceanSound);
    UpdateMusicStream(beachSound);
    UpdateMusicStream(mountainSound);
    UpdateMusicStream(fieldSound);
    UpdateMusicStream(forestSound);
    UpdateMusicStream(radiationSound);

    Color pixelColor = GetImageColor(G::perlin, static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));
    Color radiationPixel = GetImageColor(G::explosionImage, static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));

    bool isContaminated = ColorToInt(radiationPixel) != ColorToInt(Color{0, 0, 0, 0});

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
    else if (playerPos.x < 0 || playerPos.x > G::MAP_WIDTH || playerPos.y < 0 || playerPos.y > G::MAP_HEIGHT) ResumeMusicStream(oceanSound);
    else PauseMusicStream(oceanSound);

    // Beach
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[2])) ResumeMusicStream(beachSound);
    else PauseMusicStream(beachSound);

    // Field
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[3]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[4])) ResumeMusicStream(fieldSound);
    else PauseMusicStream(fieldSound);

    // Forest
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[5]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[6])) ResumeMusicStream(forestSound);
    else PauseMusicStream(forestSound);

    // Mountain
    if (ColorToInt(pixelColor) == ColorToInt(mapColors[7]) ||
        ColorToInt(pixelColor) == ColorToInt(mapColors[8])) ResumeMusicStream(mountainSound);
    else PauseMusicStream(mountainSound);
}

