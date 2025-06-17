#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 650

struct Gradient {
    int level;
    Color color;
};

Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}

const std::vector<Gradient> map = {
    {32 * 1+10, Color{90, 90, 255, 255}}, // Deep Water (Ocean, Lake etc.)
    {32 * 2+10, Color{125, 125, 255, 255}}, // Low Water
    {32 * 2+20, Color{247, 252, 204, 255}}, // Beach
    {32 * 3+20, Color{129, 245, 109, 255}}, // Grass
    {32 * 4+10, Color{117, 219, 99, 255}},
    {32 * 5+20, Color{97, 184, 81, 255}},
    {32 * 6, Color{191, 191, 191, 255}},
    {32 * 7-20, Color{153, 153, 153, 255}},
    {32 * 8, Color{255, 255, 255, 255}},
        };

void proceedMap(Image *image, const std::vector<struct Gradient> &array)
{
    ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    unsigned char *pixels = (unsigned char *)image->data;
    int width = image->width;
    int height = image->height;

    Image newImage = GenImageColor(width, height, BLACK);
    Color *newPixels = LoadImageColors(newImage);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            unsigned char value = pixels[index];

            Color color = map.back().color;
            for (const auto &g : map) {
                if (value < g.level) {
                    color = g.color;
                    break;
                }
            }

            newPixels[index] = color;
        }
    }

    Image resultImage = {
        .data = newPixels,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    UnloadImage(*image);
    *image = resultImage;
}

float FalloffValue(float x, float y) // Falloff Function
{
    float value = sqrtf(x * x + y * y);
    float a = 1.4f; // "Zerstreuung" and little islands
    float b = 7.5f; // Makes the island THICCCC
    return powf(value, a) / (powf(value, a) + powf(b - b * value, a));
}

std::vector<std::vector<float>> GenerateFalloffMap(int width, int height) // Falloff Calculation
{
    std::vector<std::vector<float>> map(height, std::vector<float>(width));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float nx = x / (float)width * 2.0f - 1.0f;
            float ny = y / (float)height * 2.0f - 1.0f;
            map[y][x] = FalloffValue(nx, ny);
        }
    }

    return map;
}

void ApplyFalloffToImage(Image *image, const std::vector<std::vector<float>> &falloffMap) // Apply falloff
{
    ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    unsigned char *pixels = (unsigned char *)image->data;
    int width = image->width;
    int height = image->height;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * width + x;
            float val = pixels[index] / 255.0f;
            val -= falloffMap[y][x];
            val = Clamp(val, 0.0f, 1.0f);
            pixels[index] = (unsigned char)(val * 255);
        }
    }
}

int main() {
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");

    SetTargetFPS(60);

    Image perlin = GenImagePerlinNoise(SCREEN_WIDTH, SCREEN_HEIGHT, static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_WIDTH/2), static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_HEIGHT/2), 6);

    std::vector<std::vector<float>> falloff = GenerateFalloffMap(SCREEN_WIDTH, SCREEN_HEIGHT);

    ApplyFalloffToImage(&perlin, falloff); // finally use falloff

    /*
    const std::vector<Gradient> map = {
{32 * 1+10, Color{90, 90, 255, 255}}, // Deep Water (Ocean, Lake etc.)
{32 * 2+10, Color{125, 125, 255, 255}}, // Low Water
{32 * 2+20, Color{247, 252, 204, 255}}, // Beach
{32 * 3+20, Color{129, 245, 109, 255}}, // Grass
{32 * 4+10, Color{117, 219, 99, 255}},
{32 * 5+20, Color{97, 184, 81, 255}},
{32 * 6, Color{191, 191, 191, 255}},
{32 * 7-20, Color{153, 153, 153, 255}},
{32 * 8, Color{255, 255, 255, 255}},
    };
    */

    proceedMap(&perlin, map);

    // ImageResizeNN(&perlin, SCREEN_WIDTH, SCREEN_HEIGHT);

    const Texture2D perlinTexture = LoadTextureFromImage(perlin);

    const Image ant = LoadImage("images/ant.png");
    const Texture2D antTexture = LoadTextureFromImage(ant);

    Vector2 mousePos = GetMousePosition();
    Vector2 antPos = mousePos;
    float lerpAmount = 0.22f;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        mousePos = GetMousePosition();
        antPos = Vector2Lerp(antPos, mousePos, lerpAmount);

        DrawTextureV(antTexture, antPos, WHITE);

        DrawFPS(GetScreenWidth() - 100, GetScreenHeight() - 25);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}