//
// Created by minhp on 15.08.2025.
//

#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H
#include <vector>
#include <map>
#include <utility>

#include "raylib.h"

struct Chunk {
    int x, y;
    Texture2D texture;
};

class ChunkGeneration {
    public:
        static std::map<std::pair<int, int>, Chunk> chunkMap;

        static constexpr int chunkSize = 1028;
        static constexpr int chunkAmountX = 4;
        static constexpr int chunkAmountY = 4;

        static void InitFalloff();
        static int GetChunkSize();
        static Chunk GenerateChunk(int chunkX, int chunkY);
        static std::vector<Chunk*> GetVisibleChunks(const Camera2D& camera);
        static void DrawChunks(const std::vector<Chunk*>& chunks);

        static std::vector<std::vector<float>> globalFalloff;
    };



#endif //CHUNKGENERATION_H
