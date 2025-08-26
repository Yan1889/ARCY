//
// Created by yanam on 26.08.2025.
//

#include "Buildings.h"
#include "../Globals.h"

#include <iostream>

using namespace Buildings;

int Buildings::GetCost(const BUILDING_TYPE type) {
    switch (type) {
        case CITY: return cityCost;
        case SILO: return siloCost;
        case UNKNOWN: std::cerr << "[Error]" << std::endl;
    }
    return -1;
}

Texture2D &Buildings::GetBuildingTexture(const BUILDING_TYPE t) {
    const bool zoomedIn = G::maxZoomForDetailedBuildings < G::camera.zoom;

    switch (t) {
        case CITY:
            return zoomedIn? TextureCollection::cityZoomed: TextureCollection::city;
        case SILO:
            return zoomedIn? TextureCollection::siloZoomed: TextureCollection::silo;
        case UNKNOWN:
            std::cerr << "[Error] cannot get unknown type" << std::endl;
    }

    // to not get compiler warnings
    return TextureCollection::city;
}