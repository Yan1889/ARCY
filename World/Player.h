//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <queue>
#include <unordered_set>
#include <vector>

#include "Money.h"
#include "raylib.h"
#include "Map/PixelRef.h"


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
    std::vector<PixelRef> _cityPositions;
    float _growth{};
    float _growthFactor = 0.0085f;

    // money
    float _cooldownTime = 1.0f;
    float _lastActionTime;
    Money _money;

    // territory
    std::unordered_set<PixelRef, PixelRef::Hasher> _allPixels;
    std::vector<PixelRef> _borderPixels;
    std::unordered_set<PixelRef, PixelRef::Hasher> _borderSet;

    // attack
    std::vector<int> _peopleWorkingOnAttack;
    // { {attackedPlayerId, { {prio1, pixel1}, {prio2, pixel2}, ... } ... }
    std::vector<std::pair<int, std::priority_queue<std::pair<float, PixelRef>>>> _allOnGoingAttackQueues;
    std::vector<std::unordered_set<PixelRef, PixelRef::Hasher>> _pixelsQueuedUp;

    PixelRef _allPixelsSummed;
    PixelRef _centerPixel;


    Player(Pixel startPos, int startRadius);

    void Expand(int target, float percentage);

    void ProcessAttackQueue(int queueIdx);

    static float GetInvasionAcceptP(const Color& terrainColor);

    void GetOwnershipOfPixel(int x, int y);

    float GetPriorityOfPixel(PixelRef p, int targetId) const;

    void Update();
    void GrowPopulation();
    void IncreaseMoney();
    void AddCity(const Vector2& pos);
    void AddCity(const PixelRef& pos);
};


#endif //POPULATION_H
