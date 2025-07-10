//
// Created by yanam on 18.06.2025.
//

#include "TextureCollection.h"

Texture2D TextureCollection::city = {};

void TextureCollection::LoadAll() {
    // Use 2nd option for web, else 1st
    // city = LoadTexture("assets/images/city.png");
    // city = LoadTexture("city.png");

    // was ist hier los haha
    city = LoadTexture("images/city.png");
}

void TextureCollection::UnloadAll() {
    UnloadTexture(city);
}

