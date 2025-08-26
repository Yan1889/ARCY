//
// Created by yanam on 18.06.2025.
//

#ifndef TEXTURECOLLECTION_H
#define TEXTURECOLLECTION_H
#include "raylib.h"


namespace  TextureCollection {
    inline Texture2D city;
    inline Texture2D silo;
    inline Texture2D cityZoomed;
    inline Texture2D siloZoomed;

    inline Texture2D atomBomb;
    inline Texture2D hydrogenBomb;
    inline Texture2D explosion;
    inline Texture2D flash;

    inline Texture2D mapIcon;

    void LoadAll();
    void UnloadAll();
};



#endif //TEXTURECOLLECTION_H
