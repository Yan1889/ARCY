//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

#include <iostream>
#include <ostream>

using namespace Buildings;

Texture2D &TextureCollection::GetBuildingTexture(const BUILDING_TYPE t) {
    switch (t) {
        case CITY:
            return city;
        case SILO:
            return silo;
        case UNKNOWN:
            std::cerr << "[Error] cannot get unknown type" << std::endl;
    }

    // to not get compiler warnings
    return city;
}


void TextureCollection::LoadAll() {
    // Normal: "assets/images/..." Web: "..."
    city = LoadTexture("assets/images/city.png");
    silo = LoadTexture("assets/images/icbm.png");
    cityZoomed = LoadTexture("assets/images/city_zoomed.png");
    siloZoomed = LoadTexture("assets/images/misslesilo_zoomed.png");

    atomBomb = LoadTexture("assets/images/atomBomb.png");
    hydrogenBomb = LoadTexture("assets/images/hydrogenBomb.png");
    explosion = LoadTexture("assets/images/explosion.png");
    flash = LoadTexture("assets/images/flash.png");

    mapIcon = LoadTexture("assets/images/mapIcon.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
    UnloadTexture(silo);
    UnloadTexture(cityZoomed);
    UnloadTexture(siloZoomed);

    UnloadTexture(atomBomb);
    UnloadTexture(hydrogenBomb);
    UnloadTexture(explosion);
    UnloadTexture(flash);

    UnloadTexture(mapIcon);
}

