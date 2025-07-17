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
#include "buildings/City.h"
#include "map/Pixel.h"
#include "buildings/MissileSilo.h"


struct AttackQueue {
    int targetPlayerId;
    int troops;
    std::queue<Pixel *> queue;
};

class Player {
public:
    int _id;
    Color _color;

    bool _dead{};

    // population
    int _population = 1000;
    int _maxPopulation{};
    float _growthCooldown = 100;
    float _growthRate = 100;
    float _growth{};
    float _growthFactor = 0.0085f;

    // buildings
    std::vector<City> _cities;
    std::vector<MissileSilo> _silos;

    // money
    float _cooldownTime = 1.0f;
    float _lastActionTime;
    Money _money;

    // territory
    std::unordered_set<Pixel *> _allPixels;
    std::deque<Pixel *> _borderPixels;
    std::unordered_set<Pixel *> _borderSet;
    std::unordered_set<Pixel *> _dirtyPixels;

    // attack
    std::unordered_map<int, AttackQueue> _targetToAttackMap;

    int _allPixelsSummed_x{};
    int _allPixelsSummed_y{};
    int _centerPixel_x{};
    int _centerPixel_y{};

    Player(Pixel* startPos, int startRadius);

    void Update();
    void Expand(int target, float percentage);

    std::unordered_set<Pixel *> ReFillAttackQueueFromScratch(AttackQueue& attackQueue);
    void ProcessAttackQueue(AttackQueue& attackQueue);
    void GetOwnershipOfPixel(Pixel* newP);
    void LoseOwnershipOfPixel(Pixel * pixel, bool updateTextureToo);

    void UpdateAllDirty();
    void MarkPixelAsDirty(Pixel* pixel);
    void UpdateSingleDirty(Pixel* pixel);
    void AddBorderPixel(Pixel* pixel);
    void RemoveBorderPixel(Pixel* pixel);

    void GrowPopulation();
    void IncreaseMoney();
    void RemovePixelFromCenter(Pixel* newP);
    void AddPixelToCenter(Pixel* newP);
    void AddCity(const Vector2& pos);
    void AddCity(Pixel* pos);
    Pixel* GetNearestCityFromPixel(Pixel* point) const;
};


#endif //POPULATION_H
