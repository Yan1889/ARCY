//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

Texture2D TextureCollection::city = {};

void TextureCollection::LoadAll() {
    city = LoadTexture("src/images/city.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
}

