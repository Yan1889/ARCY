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

ChunkGeneration::ChunkGeneration(int size, int x, int y)
{
    chunkSize = size;
    chunkAmountX = x;
    chunkAmountY = y;
    worldWidth = chunkAmountX * chunkSize;
    worldHeight = chunkAmountY * chunkSize;
}

void ChunkGeneration::InitFalloff()
{
    globalFalloff = PerlinNoise::GenerateFalloffMap(worldWidth, worldHeight);
}

Chunk ChunkGeneration::GenerateChunk(int chunkX, int chunkY)
{
    Chunk chunk;
    chunk.x = chunkX;
    chunk.y = chunkY;
    chunk.generated = true;

    int worldOffsetX = chunkX * chunkSize;
    int worldOffsetY = chunkY * chunkSize;

    Image perlin = GenImagePerlinNoise(
        chunkSize, chunkSize,
        worldOffsetX, worldOffsetY,
        6
        );

    std::vector<std::vector<float>> chunkFalloff(chunkSize, std::vector<float>(chunkSize));
    for (int y = 0; y < chunkSize; y++)
    {
        for (int x = 0; x < chunkSize; x++)
        {
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

std::vector<Chunk*> ChunkGeneration::GetVisibleChunks(Camera2D camera, float screenWidth, float screenHeight)
{
    std::vector<Chunk*> visibleChunks;

    float camX = camera.target.x - screenWidth / 2 / camera.zoom;
    float camY = camera.target.y - screenHeight / 2 / camera.zoom;

    const int CHUNK_BUFFER = 0;

    int minChunkX = std::max(0, (int)floor(camX / chunkSize) - CHUNK_BUFFER);
    int maxChunkX = std::min(chunkAmountX - 1, (int)floor((camX + screenWidth / camera.zoom) / chunkSize) + CHUNK_BUFFER);
    int minChunkY = std::max(0, (int)floor(camY / chunkSize) - CHUNK_BUFFER);
    int maxChunkY = std::min(chunkAmountY - 1, (int)floor((camY + screenHeight / camera.zoom) / chunkSize) + CHUNK_BUFFER);

    for (int y = minChunkY; y <= maxChunkY; y++)
    {
        for (int x = minChunkX; x <= maxChunkX; x++)
        {
            auto key = std::make_pair(x, y);

            if (chunkMap.find(key) == chunkMap.end())
            {
                Chunk& c = chunkMap[key];
                c.x = x;
                c.y = y;
                c.generated = true;

                int worldOffsetX = x * chunkSize;
                int worldOffsetY = y * chunkSize;

                Image perlin = GenImagePerlinNoise(chunkSize, chunkSize, worldOffsetX, worldOffsetY, 6);

                std::vector<std::vector<float>> chunkFalloff(chunkSize, std::vector<float>(chunkSize));
                for (int yy = 0; yy < chunkSize; yy++)
                {
                    for (int xx = 0; xx < chunkSize; xx++)
                    {
                        int wx = worldOffsetX + xx;
                        int wy = worldOffsetY + yy;
                        chunkFalloff[yy][xx] = globalFalloff[wy][wx];
                    }
                }

                PerlinNoise::ApplyFalloffToImage(&perlin, chunkFalloff);
                PerlinNoise::proceedMap(&perlin);

                c.texture = LoadTextureFromImage(perlin);
                UnloadImage(perlin);
                std::cout << "Generated chunk: " << x << "," << y << std::endl;
            }

            visibleChunks.push_back(&chunkMap[key]);
        }
    }

    return visibleChunks;
}

void ChunkGeneration::DrawChunks(const std::vector<Chunk*>& chunks)
{
    for (auto* chunk : chunks)
    {
        if (!chunk->generated) continue;

        Rectangle src = {0, 0, static_cast<float>(chunkSize), static_cast<float>(chunkSize)};
        Rectangle dest = {static_cast<float>(chunk->x * chunkSize), static_cast<float>(chunk->y * chunkSize), static_cast<float>(chunkSize), static_cast<float>(chunkSize)};
        DrawTexturePro(chunk->texture, src, dest, {0, 0}, 0.0f, WHITE);
    }
}



