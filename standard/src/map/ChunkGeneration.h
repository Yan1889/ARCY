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
    Image image;
    bool generated = false;
};

    class ChunkGeneration {
    public:
        static std::map<std::pair<int, int>, Chunk> chunkMap;

        static int chunkSize;
        static int chunkAmountX;
        static int chunkAmountY;

        static int worldWidth;
        static int worldHeight;

        static void InitChunkGeneration(int x, int y, int size);

        static void InitFalloff();
        static int GetChunkSize();
        static Chunk GenerateChunk(int chunkX, int chunkY);
        static std::vector<Chunk*> GetVisibleChunks(const Camera2D& camera, float screenWidth, float screenHeight);
        static void DrawChunks(const std::vector<Chunk*>& chunks);
        static Image GenerateChunkImage(int chunkX, int chunkY);

        static std::vector<std::vector<float>> globalFalloff;
    };



#endif //CHUNKGENERATION_H
