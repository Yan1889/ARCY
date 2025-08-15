//
// Created by minhp on 15.08.2025.
//

#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H
#include <vector>
#include <map>
#include <utility>

#include "raylib.h"

struct Chunk
{
    int x;
    int y;
    Texture2D texture;
    bool generated = false;
};

    class ChunkGeneration {
    public:
        std::map<std::pair<int, int>, Chunk> chunkMap;

        int chunkSize;
        int chunkAmountX;
        int chunkAmountY;

        int worldWidth;
        int worldHeight;

        ChunkGeneration(int size, int x, int y);

        void InitFalloff();
        Chunk GenerateChunk(int chunkX, int chunkY);
        std::vector<Chunk*> GetVisibleChunks(Camera2D camera, float screenWidth, float screenHeight);
        void DrawChunks(const std::vector<Chunk*>& chunks);
        Image GenerateChunkImage(int chunkX, int chunkY);

        std::vector<std::vector<float>> globalFalloff;
    };



#endif //CHUNKGENERATION_H
