//
// Created by minhp on 12.08.2025.
//

#ifndef CAMERACLIPPING_H
#define CAMERACLIPPING_H
#include "raylib.h"
#include "Pixel.h"

namespace CameraClipping
{
    inline bool IsPixelVisible(const Pixel* p, const Rectangle& viewRect)
    {
        return (p->x >= viewRect.x && p->x <= viewRect.x + viewRect.width) &&
               (p->y >= viewRect.y && p->y <= viewRect.y + viewRect.height);
    }

    inline bool CheckCollisionCameraRec(const Rectangle& buildingRect, const Rectangle& viewRect)
    {
        return (buildingRect.x < viewRect.x + viewRect.width) && (buildingRect.x + buildingRect.width > viewRect.x) &&
               (buildingRect.y < viewRect.y + viewRect.height) && (buildingRect.y + buildingRect.height > viewRect.y);
    }

    inline bool CheckCollisionCameraCircle(const Vector2& center, float radius, const Rectangle& viewRect)
    {
        return (center.x + radius < viewRect.x ||
             center.x - radius > viewRect.x + viewRect.width ||
             center.y + radius < viewRect.y ||
             center.y - radius > viewRect.y + viewRect.height);
    }

    inline Rectangle GetViewRectangle(const Camera2D& camera)
    {
        return Rectangle {
            camera.target.x - camera.offset.x / camera.zoom,
            camera.target.y - camera.offset.y / camera.zoom,
            static_cast<float>(GetScreenWidth()) / camera.zoom,
            static_cast<float>(GetScreenHeight()) / camera.zoom
        };
    }
}



#endif //CAMERACLIPPING_H
