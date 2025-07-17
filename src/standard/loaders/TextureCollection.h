//
// Created by yanam on 18.06.2025.
//

#ifndef TEXTURECOLLECTION_H
#define TEXTURECOLLECTION_H
#include "raylib.h"


namespace  TextureCollection {
    inline Texture2D city;
    inline Texture2D atomBomb;
    inline Texture2D hydrogenBomb;
    inline Texture2D explosion;
    inline Texture2D flash;

    void LoadAll();
    void UnloadAll();
};



#endif //TEXTURECOLLECTION_H
