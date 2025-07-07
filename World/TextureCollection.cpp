//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

Texture2D TextureCollection::city = {};

void TextureCollection::LoadAll() {
    // Use 2nd option for web, else 1st
    //city = LoadTexture("src/images/city.png");
    city = LoadTexture("city.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
}

