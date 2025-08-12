//
// Created by yanam on 18.06.2025.
//

#include "PerlinNoise.h"


#include "raymath.h"
#include "TerrainKind.h"

void PerlinNoise::proceedMap(Image *image) {
    ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    const unsigned char *pixels = static_cast<unsigned char *>(image->data);
    const int width = image->width;
    const int height = image->height;

    Image newImage = GenImageColor(width, height, BLACK);
    Color *newPixels = LoadImageColors(newImage);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            unsigned char value = pixels[index];

            Color color = Terrain::mapParts.back().color;
            for (const auto &g: Terrain::mapParts) {
                if (value < g.level) {
                    color = g.color;
                    break;
                }
            }

            newPixels[index] = color;
        }
    }

    const Image resultImage = {
        .data = newPixels,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    UnloadImage(*image);
    *image = resultImage;
}

float PerlinNoise::FalloffValue(float x, float y) // Falloff Function
{
    float value = sqrtf(x * x + y * y);
    float a = 1.4f; // "Zerstreuung" and little islands
    float b = 7.5f; // Makes the island THICCCC
    return powf(value, a) / (powf(value, a) + powf(b - b * value, a));
}

std::vector<std::vector<float> > PerlinNoise::GenerateFalloffMap(int width, int height) // Falloff Calculation
{
    std::vector<std::vector<float> > map(height, std::vector<float>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x / (float) width * 2.0f - 1.0f;
            float ny = y / (float) height * 2.0f - 1.0f;
            map[y][x] = FalloffValue(nx, ny);
        }
    }

    return map;
}

void PerlinNoise::ApplyFalloffToImage(Image *image, const std::vector<std::vector<float> > &falloffMap) // Apply falloff
{
    ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    unsigned char *pixels = static_cast<unsigned char *>(image->data);
    int width = image->width;
    int height = image->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            float val = pixels[index] / 255.0f;
            val -= falloffMap[y][x];
            val = Clamp(val, 0.0f, 1.0f);
            pixels[index] = static_cast<unsigned char>(val * 255);
        }
    }
}