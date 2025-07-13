//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Money.h"
#include "raylib.h"
#include "Map/Pixel.h"


struct AttackQueue {
    int targetPlayerId;
    std::priority_queue<std::pair<float, Pixel *>> queue;
};

class Player {
public:
    int _id;
    Color _color{};

    // population
    int _population = 100;
    int _maxPopulation{};
    float _growthCooldown = 100;
    float _growthRate = 100;
    int _cityCount{};
    std::vector<Pixel*> _cityPositions;
    float _growth{};
    float _growthFactor = 0.0085f;

    // money
    float _cooldownTime = 1.0f;
    float _lastActionTime;
    Money _money;

    // territory
    std::unordered_set<Pixel *> _allPixels;
    std::vector<Pixel *> _borderPixels;
    std::unordered_set<Pixel *> _borderSet;

    // attack
    std::vector<int> _peopleWorkingOnAttack;
    std::unordered_map<int, int> _attackedPlayerIdToQueueIdxMap;
    // { {attackedPlayerId, { {prio1, pixel1}, {prio2, pixel2}, ... } ... }
    std::vector<AttackQueue> _allOnGoingAttackQueues;
    std::vector<std::unordered_set<Pixel *>> _pixelsQueuedUp;

    int _allPixelsSummed_x{};
    int _allPixelsSummed_y{};
    int _centerPixel_x{};
    int _centerPixel_y{};


    Player(Pixel* startPos, int startRadius);

    void Expand(int target, float percentage);

    void ProcessAttackQueue(int queueIdx);

    static float GetInvasionAcceptP(const Color& terrainColor);

    void GetOwnershipOfPixel(Pixel* newP);

    float GetPriorityOfPixel(Pixel* p, int targetId) const;

    void Update();
    void GrowPopulation();
    void IncreaseMoney();

    void UpdateBorderAroundPixel(Pixel* pixel);
    void UpdateBorderStatusOfPixel(Pixel* pixel);
    void AddBorderPixel(Pixel* pixel);
    void RemoveBorderPixel(Pixel* pixel);

    void RecalculateCenterPixel();
    void AddCity(const Vector2& pos);
    void AddCity(Pixel* pos);
};


#endif //POPULATION_H
