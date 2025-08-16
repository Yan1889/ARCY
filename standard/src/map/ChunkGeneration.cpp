//
// Created by minhp on 15.08.2025.
//
#include "PerlinNoise.h"
#include "TerrainKind.h"
#include "ChunkGeneration.h"

#include <cmath>
#include <iostream>
#include <ostream>

using namespace Terrain;

int ChunkGeneration::chunkSize = 1028;
int ChunkGeneration::chunkAmountX = 0;
int ChunkGeneration::chunkAmountY = 0;
int ChunkGeneration::worldHeight = 0;
int ChunkGeneration::worldWidth = 0;
std::map<std::pair<int, int>, Chunk> ChunkGeneration::chunkMap{};
std::vector<std::vector<float>> ChunkGeneration::globalFalloff{};

void ChunkGeneration::InitChunkGeneration(const int x, const int y, const int size) {
    chunkAmountX = x;
    chunkAmountY = y;
    chunkSize = size;
    worldWidth = chunkAmountX * chunkSize;
    worldHeight = chunkAmountY * chunkSize;
}

void ChunkGeneration::InitFalloff() {
    globalFalloff = PerlinNoise::GenerateFalloffMap(worldWidth, worldHeight);
}

int ChunkGeneration::GetChunkSize()
{
    return chunkSize;
}

Chunk ChunkGeneration::GenerateChunk(int chunkX, int chunkY) {
    Chunk chunk{
        .x = chunkX,
        .y = chunkY,
        .generated = true
    };

    int worldOffsetX = chunkX * chunkSize;
    int worldOffsetY = chunkY * chunkSize;

    Image perlin = GenImagePerlinNoise(
        chunkSize, chunkSize,
        worldOffsetX, worldOffsetY,
        6
    );

    std::vector<std::vector<float> > chunkFalloff(chunkSize, std::vector<float>(chunkSize));
    for (int y = 0; y < chunkSize; y++) {
        for (int x = 0; x < chunkSize; x++) {
            int wx = worldOffsetX + x;
            int wy = worldOffsetY + y;
            chunkFalloff[y][x] = globalFalloff[wy][wx];
        }
    }

    PerlinNoise::ApplyFalloffToImage(&perlin, chunkFalloff);
    PerlinNoise::proceedMap(&perlin);

    chunk.texture = LoadTextureFromImage(perlin);
    UnloadImage(perlin);

    return chunk;
}

std::vector<Chunk *> ChunkGeneration::GetVisibleChunks(const Camera2D &camera, float screenWidth, float screenHeight) {
    std::vector<Chunk *> visibleChunks;

    const float camX = camera.target.x - screenWidth / 2 / camera.zoom;
    const float camY = camera.target.y - screenHeight / 2 / camera.zoom;

    const int CHUNK_BUFFER = 0;

    const int minChunkX = std::max(
        0,
        static_cast<int>(floor(camX / chunkSize)) - CHUNK_BUFFER
    );
    const int maxChunkX = std::min(
        chunkAmountX - 1,
        static_cast<int>(floor((camX + screenWidth / camera.zoom) / chunkSize)) + CHUNK_BUFFER
    );
    const int minChunkY = std::max(
        0,
        static_cast<int>(floor(camY / chunkSize)) - CHUNK_BUFFER
    );
    const int maxChunkY = std::min(
        chunkAmountY - 1,
        static_cast<int>(floor((camY + screenHeight / camera.zoom) / chunkSize)) + CHUNK_BUFFER
    );

    for (int y = minChunkY; y <= maxChunkY; y++) {
        for (int x = minChunkX; x <= maxChunkX; x++) {
            auto key = std::make_pair(x, y);
            std::cout << chunkMap.size() << std::endl;

            auto it = chunkMap.find(key);
            if (it == chunkMap.end()) {
                Chunk c{
                    .x = x,
                    .y = y,
                    .generated = true,
                };

                const int worldOffsetX = x * chunkSize;
                const int worldOffsetY = y * chunkSize;
                Image perlin = GenImagePerlinNoise(chunkSize, chunkSize, worldOffsetX, worldOffsetY, 6);

                std::vector<std::vector<float> > chunkFalloff(chunkSize, std::vector<float>(chunkSize));
                for (int yy = 0; yy < chunkSize; yy++) {
                    for (int xx = 0; xx < chunkSize; xx++) {
                        int wx = worldOffsetX + xx;
                        int wy = worldOffsetY + yy;
                        chunkFalloff[yy][xx] = globalFalloff[wy][wx];
                    }
                }

                PerlinNoise::ApplyFalloffToImage(&perlin, chunkFalloff);
                PerlinNoise::proceedMap(&perlin);

                c.texture = LoadTextureFromImage(perlin);
                c.image = ImageCopy(perlin);
                UnloadImage(perlin);

                it = chunkMap.insert({key, c}).first;
            }

            visibleChunks.push_back(&it->second);
        }
    }

    return visibleChunks;
}

void ChunkGeneration::DrawChunks(const std::vector<Chunk *> &chunks) {
    for (auto *chunk: chunks) {
        if (!chunk->generated) continue;

        Rectangle src = {0, 0, static_cast<float>(chunkSize), static_cast<float>(chunkSize)};
        Rectangle dest = {
            static_cast<float>(chunk->x * chunkSize), static_cast<float>(chunk->y * chunkSize),
            static_cast<float>(chunkSize), static_cast<float>(chunkSize)
        };
        DrawTexturePro(chunk->texture, src, dest, {0, 0}, 0.0f, WHITE);
    }
}
