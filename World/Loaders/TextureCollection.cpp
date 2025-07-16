//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

void TextureCollection::LoadAll() {
    // Normal: "assets/images/..." Web: "..."
    city = LoadTexture("assets/images/city.png");
    atomBomb = LoadTexture("assets/images/atomBomb.png");
    hydrogenBomb = LoadTexture("assets/images/hydrogenBomb.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
    UnloadTexture(atomBomb);
    UnloadTexture(hydrogenBomb);
}

