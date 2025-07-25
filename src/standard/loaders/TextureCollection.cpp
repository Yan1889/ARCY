//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

void TextureCollection::LoadAll() {
    // Normal: "assets/images/..." Web: "..."
    city = LoadTexture("assets/images/city.png");
    atomBomb = LoadTexture("assets/images/atomBomb.png");
    hydrogenBomb = LoadTexture("assets/images/hydrogenBomb.png");
    explosion = LoadTexture("assets/images/explosion.png");
    flash = LoadTexture("assets/images/flash.png");
    silo = LoadTexture("assets/images/icbm.png");
    mapIcon = LoadTexture("assets/images/mapIcon2.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
    UnloadTexture(silo);
    UnloadTexture(atomBomb);
    UnloadTexture(hydrogenBomb);
    UnloadTexture(explosion);
    UnloadTexture(flash);
    UnloadTexture(mapIcon);
}

