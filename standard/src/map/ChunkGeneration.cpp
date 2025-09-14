//
// Created by minhp on 15.08.2025.
//
#include "PerlinNoise.h"
#include "TerrainKind.h"
#include "ChunkGeneration.h"

#include <cmath>
#include <ostream>
#include <set>

#include "../Globals.h"

std::map<std::pair<int, int>, Chunk> ChunkGeneration::chunkMap{};
std::vector<std::vector<float> > ChunkGeneration::globalFalloff{};

void ChunkGeneration::InitFalloff() {
    globalFalloff = PerlinNoise::GenerateFalloffMap(G::MAP_WIDTH, G::MAP_HEIGHT);
}

void ChunkGeneration::ProcessFinishedChunks() {
    std::lock_guard<std::mutex> lock(finishedMutex);

    while (!finishedQueue.empty()) {
        PendingChunk pc = finishedQueue.front();
        finishedQueue.pop();

        Texture2D tex = LoadTextureFromImage(pc.image);
        UnloadImage(pc.image);

        chunkMap[{pc.x, pc.y}] = Chunk{ pc.x, pc.y, tex };
    }
}

Image ChunkGeneration::GenerateChunkImage(int chunkX, int chunkY) {
    const int worldOffsetX = chunkX * chunkSize;
    const int worldOffsetY = chunkY * chunkSize;

    Image perlinImage = GenImagePerlinNoise(
        chunkSize, chunkSize,
        worldOffsetX, worldOffsetY,
        1 // Default 6
    );

    if (useFalloff) {
        std::vector<std::vector<float>> chunkFalloff(chunkSize, std::vector<float>(chunkSize));
        for (int y = 0; y < chunkSize; y++) {
            for (int x = 0; x < chunkSize; x++) {
                const int wx = worldOffsetX + x;
                const int wy = worldOffsetY + y;
                chunkFalloff[y][x] = globalFalloff[wy][wx];
            }
        }
        PerlinNoise::ApplyFalloffToImage(&perlinImage, chunkFalloff);
    }

    PerlinNoise::proceedMap(&perlinImage);

    // pixels
    const auto pixels = static_cast<Color *>(perlinImage.data);
    for (int x = worldOffsetX; x < worldOffsetX + chunkSize; x++) {
        for (int y = worldOffsetY; y < worldOffsetY + chunkSize; y++) {
            const int lx = x % chunkSize;
            const int ly = y % chunkSize;
            Color color = pixels[ly * perlinImage.width + lx];

            G::PixelAt(x, y)->Load(color);
        }
    }

    return perlinImage;
}

std::vector<Chunk *> ChunkGeneration::GetVisibleChunks(const Camera2D &camera) {
    const float screenW = GetScreenWidth();
    const float screenH = GetScreenHeight();

    std::vector<Chunk *> visibleChunks;

    const float camX = camera.target.x - screenW / 2 / camera.zoom;
    const float camY = camera.target.y - screenH / 2 / camera.zoom;

    const int CHUNK_BUFFER = 0;

    const int minChunkX = std::max(
        0,
        static_cast<int>(floor(camX / chunkSize)) - CHUNK_BUFFER
    );
    const int maxChunkX = std::min(
        chunkAmountX - 1,
        static_cast<int>(floor((camX + screenW / camera.zoom) / chunkSize)) + CHUNK_BUFFER
    );
    const int minChunkY = std::max(
        0,
        static_cast<int>(floor(camY / chunkSize)) - CHUNK_BUFFER
    );
    const int maxChunkY = std::min(
        chunkAmountY - 1,
        static_cast<int>(floor((camY + screenH / camera.zoom) / chunkSize)) + CHUNK_BUFFER
    );

    for (int y = minChunkY; y <= maxChunkY; y++) {
        for (int x = minChunkX; x <= maxChunkX; x++) {
            auto key = std::make_pair(x, y);

            {
                // std::lock_guard<std::mutex> lock(mapMutex);
                auto it = chunkMap.find(key);
                if (it != chunkMap.end()) {
                    visibleChunks.push_back(&it->second);
                    continue;
                }
            }

            {
                /* std::lock_guard<std::mutex> lock(jobMutex);
                if (jobsSet.insert(key).second) {
                    jobQueue.push(key);
                    jobCv.notify_one();
                } */

                Image image = GenerateChunkImage(x, y);
                Texture2D tex = LoadTextureFromImage(image);
                UnloadImage(image);

                chunkMap[{x, y}] = Chunk{ x, y, tex };
                visibleChunks.push_back(&chunkMap[{x, y}]);
            }
        }
    }

    return visibleChunks;
}

void ChunkGeneration::DrawChunks(const std::vector<Chunk *> &chunks) {
    for (auto *chunk: chunks) {
        Rectangle src = {0, 0, static_cast<float>(chunkSize), static_cast<float>(chunkSize)};
        Rectangle dest = {
            static_cast<float>(chunk->x * chunkSize), static_cast<float>(chunk->y * chunkSize),
            static_cast<float>(chunkSize), static_cast<float>(chunkSize)
        };
        DrawTexturePro(chunk->texture, src, dest, {0, 0}, 0.0f, WHITE);
    }
}
