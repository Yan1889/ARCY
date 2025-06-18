#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 650

#define MAP_HEIGHT 5000
#define MAP_WIDTH 5000

struct Gradient {
    int level;
    Color color;
};

Color grayScale(const unsigned char gray) {
    return Color{gray, gray, gray, 255};
}

const std::vector<Gradient> map = {
    {32 * 1+10, Color{90, 90, 255, 255}}, // Deep Water
    {32 * 2+10, Color{125, 125, 255, 255}}, // Low Water
    {32 * 2+20, Color{247, 252, 204, 255}}, // Beach
    {32 * 3+20, Color{129, 245, 109, 255}}, // Open Field
    {32 * 4+10, Color{117, 219, 99, 255}}, // Hills
    {32 * 5+20, Color{97, 184, 81, 255}}, // Forest
    {32 * 6, Color{191, 191, 191, 255}}, // Stone
    {32 * 7-20, Color{153, 153, 153, 255}}, // Mountain
    {32 * 8, Color{255, 255, 255, 255}}, // Snow
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

int main()
{
    srand(time(nullptr));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ARCY - An epic game");

    Vector2 playerPos = { MAP_WIDTH / 2, MAP_HEIGHT / 2 };

    // Camera
    Camera2D camera = {0};
    camera.target = playerPos;
    camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    //

    Image perlin = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_WIDTH/2), static_cast<int>(rand() * 10000.0f / RAND_MAX)*(SCREEN_HEIGHT/2), 6);

    std::vector<std::vector<float>> falloff = GenerateFalloffMap(MAP_WIDTH, MAP_HEIGHT);

    ApplyFalloffToImage(&perlin, falloff); // finally use falloff

    proceedMap(&perlin, map);

    const Texture2D perlinTexture = LoadTextureFromImage(perlin);

    const Image ant = LoadImage("images/ant.png");
    const Texture2D antTexture = LoadTextureFromImage(ant);

    Vector2 mousePos = GetMousePosition();
    Vector2 antPos = mousePos;
    const float lerpAmount = 0.88f;

    const int moveSpeed = 100;
    const int zoomSpeed = 5;

    const float zoomMin = 0.1f;
    const float zoomMax = 10.0f;

    const Vector2 centerPos = playerPos;

    while (!WindowShouldClose())
    {
        // Camera Movement
        if (IsKeyDown(KEY_W)) playerPos.y -= moveSpeed * GetFrameTime() * 1 / camera.zoom;
        if (IsKeyDown(KEY_S)) playerPos.y += moveSpeed * GetFrameTime() * 1 /camera.zoom;
        if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed * GetFrameTime() * 1 /camera.zoom;
        if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed * GetFrameTime() * 1 / camera.zoom;

        camera.target = playerPos;

        if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed * GetFrameTime();
        if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed * GetFrameTime();

        if (camera.zoom < zoomMin) camera.zoom = zoomMin;
        if (camera.zoom > zoomMax) camera.zoom = zoomMax;
        //

        BeginDrawing();

        ClearBackground(Color{90, 90, 255, 255});

        BeginMode2D(camera);

        DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);

        mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        antPos = Vector2Lerp(antPos, mousePos, lerpAmount  * GetFrameTime());

        DrawTextureV(antTexture, antPos, WHITE);

        DrawCircleV(centerPos, 10, RED);

        EndMode2D();

        const int fps = GetFPS();
        const char* fpsText = TextFormat("FPS: %d", fps);

        int textWidth = MeasureText(fpsText, 20);
        int textX = GetScreenWidth() - textWidth - 25;
        int textY = GetScreenHeight() - 25;

        DrawText(fpsText, textX, textY, 20, DARKGREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}