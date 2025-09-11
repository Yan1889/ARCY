//
// Created by minhp on 15.08.2025.
//

#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H
#include <vector>
#include <map>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <set>

#include "raylib.h"

struct Chunk {
    int x, y;
    Texture2D texture;
};

struct PendingChunk {
    int x, y;
    Image image;
};

class ChunkGeneration {
    public:
        static std::map<std::pair<int, int>, Chunk> chunkMap;

        static constexpr int chunkSize = 100; // Default 1028
        static constexpr bool useFalloff = true;
        static constexpr int chunkAmountX = useFalloff ? 50 : 75; // Default 4 : 10
        static constexpr int chunkAmountY = useFalloff ? 50 : 75; // Default 4 : 10

        static void InitThread() {
            running = true;
            temuWorker = std::thread(WorkerLoop);
        }
        static void ShutDownThread() {
            {
                std::lock_guard<std::mutex> lock(jobMutex);
                running = false;
            }
            jobCv.notify_all();
            if (temuWorker.joinable()) temuWorker.join();
        }

        static void InitFalloff();
        static std::vector<Chunk*> GetVisibleChunks(const Camera2D& camera);
        static void DrawChunks(const std::vector<Chunk*>& chunks);

        static std::vector<std::vector<float>> globalFalloff;

        static inline std::mutex mapMutex;

        static inline std::queue<std::pair<int,int>> jobQueue;
        static inline std::set<std::pair<int,int>> jobsSet;
        static inline std::mutex jobMutex;
        static inline std::condition_variable jobCv;
        static inline std::thread temuWorker;
        static inline bool running = false;

        static inline std::queue<PendingChunk> finishedQueue;
        static inline std::mutex finishedMutex;
        static void ProcessFinishedChunks();
        static Image GenerateChunkImage(int chunkX, int chunkY);

        static void WorkerLoop() {
            while (true) {
                std::pair<int,int> job;
                {
                    std::unique_lock<std::mutex> lock(jobMutex);
                    jobCv.wait(lock, [] { return !jobQueue.empty() || !running; });

                    if (!running && jobQueue.empty()) break;

                    job = jobQueue.front();
                    jobQueue.pop();
                    jobsSet.erase(job);
                }

                Image img = GenerateChunkImage(job.first, job.second);

                {
                    std::lock_guard<std::mutex> lock(finishedMutex);
                    finishedQueue.push(PendingChunk{ job.first, job.second, img });
                }
        }
    }
};



#endif //CHUNKGENERATION_H
